#include "stdafx.h"
#include <boost/log/attributes.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <stdarg.h>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include "syslog.h"

namespace sinks = boost::log::sinks;
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;

typedef sinks::synchronous_sink<sinks::text_file_backend> file_sink;

src::severity_logger<severity_levels> logger;

template<typename C, typename T>
inline std::basic_ostream<C, T>& operator<<(
	std::basic_ostream<C, T>& s, severity_levels l)
{
	static const char* const str[] = {
		"normal",
		"warning",
		"error"
	};

	if (static_cast<std::size_t>(l) < (sizeof(str) / sizeof(*str))) {
		s << str[l];
	}
	else {
		s << static_cast<int>(l);
	}

	return s;
}

/*
 * ��־boost��س�ʼ��
*/
void log_init()
{
	// ����sinks	
	boost::shared_ptr<file_sink> fsink = boost::make_shared<file_sink>(
		keywords::file_name = "syslog%N.log",
		keywords::rotation_size = 500 * 1024 * 1024,
		keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
		keywords::auto_flush = true
	);

	// ������־�����ļ��������Ϣ
	fsink->locked_backend()->set_file_collector(sinks::file::make_collector(
		keywords::target = "log"	//Ŀ���ļ���
									//keywords::max_size = 16 * 1024 * 1024,        //������־������������С,
									//keywords::min_free_space = 100 * 1024 * 1024  //��ʹ��̿ռ�����
	));

	// ����sink��ʽ
	fsink->set_formatter(
		expr::format("[%1%][%2%] %3%")
		% expr::format_date_time< boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")	// ���ʱ�䣬���ȵ����뼶
		% expr::attr<severity_levels>("Severity")													// ������Ϣ
		% expr::smessage																			// �����Ϣ
	);

	// ���sink
	logging::core::get()->add_sink(fsink);

	// ���ͨ������
	logging::add_common_attributes();
}

/*
 * �����־��Ϣ
*/
void log_impl_print(const char* file_name, const int line, severity_levels level, const char* fmt, ...)
{
	va_list al;
	va_start(al, fmt);
	char * buffer = new char[1024];
	memset(buffer, 0, 1024);
	vsnprintf_s(buffer, 1024, 1023, fmt, al);
	std::string file = get_file_line(file_name, line);
	std::string msg = buffer;
	BOOST_LOG_SEV(logger, level) << file << " " << msg.c_str();

	delete[] buffer;
	va_end(al);
}

/*
 * �����ļ�+������Ϣ,����:[main.cpp:14]
*/
std::string get_file_line(const char* file_name, const int line)
{
	std::string s = file_name;

	int idx = s.find_last_of("\\");
	s = s.substr(idx + 1, s.length());

	char buf[100] = { 0 };
	snprintf(buf, 100, "[%s:%d]", s.c_str(), line);

	s = buf;

	return s;
}