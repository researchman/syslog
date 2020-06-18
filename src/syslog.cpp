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
 * 日志boost相关初始化
*/
void log_init()
{
	// 创建sinks	
	boost::shared_ptr<file_sink> fsink = boost::make_shared<file_sink>(
		keywords::file_name = "syslog%N.log",
		keywords::rotation_size = 500 * 1024 * 1024,
		keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
		keywords::auto_flush = true
	);

	// 设置日志保存文件夹相关信息
	fsink->locked_backend()->set_file_collector(sinks::file::make_collector(
		keywords::target = "log"	//目标文件夹
									//keywords::max_size = 16 * 1024 * 1024,        //所有日志加起来的最大大小,
									//keywords::min_free_space = 100 * 1024 * 1024  //最低磁盘空间限制
	));

	// 设置sink格式
	fsink->set_formatter(
		expr::format("[%1%][%2%] %3%")
		% expr::format_date_time< boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")	// 输出时间，精度到毫秒级
		% expr::attr<severity_levels>("Severity")													// 级别信息
		% expr::smessage																			// 输出消息
	);

	// 添加sink
	logging::core::get()->add_sink(fsink);

	// 添加通用属性
	logging::add_common_attributes();
}

/*
 * 输出日志信息
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
 * 构造文件+函数信息,例如:[main.cpp:14]
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