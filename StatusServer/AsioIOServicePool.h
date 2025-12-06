#pragma once
#include "Singleton.h"
#include <boost/asio.hpp>
class AsioIOServicePool : public Singleton<AsioIOServicePool>
{
	friend Singleton<AsioIOServicePool>;
public:
	using IOService = boost::asio::io_context;
	//告诉io_context有工作要做，防止io_context在没有工作时自动退出，使用work保持io_context的运行
	using Work = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
	using Workptr = std::unique_ptr<Work>;

	~AsioIOServicePool();
	AsioIOServicePool(const AsioIOServicePool&) = delete;
	AsioIOServicePool& operator=(const AsioIOServicePool&) = delete;

	//使用round-robin的方式返回一个io_service
	boost::asio::io_context& GetIOService();
	void Stop();
private:
	AsioIOServicePool(std::size_t size = 2/*std:;thread::hardware concurrency()*/);
	std::vector<IOService> _ioServices;
	std::vector<Workptr> _works;
	std::vector<std::thread> _threads;
	std::size_t _nextIOService;

};

