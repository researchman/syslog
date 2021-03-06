// syslog_test.cpp: 定义控制台应用程序的入口点。
//
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include "syslog.h"

void thread_func(int i)
{
	for (int j = 0; j < 1000; ++j) {
		log_info("%d thread write message, the message is i like the %d thread.", i, i);
	}
}

int main()
{
	log_init();

	std::vector<std::thread> vtp;
	for (int i = 1; i < 4; ++i) {
		std::thread t = std::thread(&thread_func, i);
		vtp.push_back(std::move(t));
	}
	
	std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
	for (size_t i = 0; i < vtp.size(); ++i) {
		vtp[i].join();
	}
	std::chrono::system_clock::time_point te = std::chrono::system_clock::now();

	std::cout << "elpase: " << (std::chrono::duration_cast<std::chrono::seconds>(te.time_since_epoch()) - std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch())).count() << "s" << std::endl;

	getchar();

    return 0;
}

