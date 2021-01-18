#ifndef boost_syslog_hpp
#define boost_syslog_hpp
#include <string>

//#define _WIN32_WINNT 0x0600

enum severity_levels {
	normal,
	warning,
	error
};

extern void log_init();

extern void log_impl_print(const char* file_name, const int line, severity_levels level, const char* fmt, ...);

#define log_info(fmt,...)  log_impl_print(__FILE__,__LINE__,normal,fmt,##__VA_ARGS__)
#define log_warn(fmt,...)  log_impl_print(__FILE__,__LINE__,warning,fmt,##__VA_ARGS__)
#define log_error(fmt,...) log_impl_print(__FILE__,__LINE__,severity_levels::error,fmt,##__VA_ARGS__)

extern std::string get_file_line(const char* file_name, const int line);

#endif