#include "AsioIOServicePool.h"

AsioIOServicePool::~AsioIOServicePool()
{
	Stop();
	std::cout << "AsionIOServicePool destruct" << std::endl;
}

boost::asio::io_context& AsioIOServicePool::GetIOService()
{
	auto& service = _ioServices[_nextIOService++];
	if (_nextIOService == _ioServices.size()) {
		_nextIOService = 0;
	}
	return service;
}

void AsioIOServicePool::Stop()
{
	for (auto& _service : _ioServices) {
		_service.stop();
	}
	for (auto& t : _threads) {
		t.join();
	}
}

AsioIOServicePool::AsioIOServicePool(std::size_t size) :
	_ioServices(size), _works(size), _nextIOService(0)
{
	for (std::size_t i{}; i < size; ++i) {

		_works[i] = std::unique_ptr<Work>(new Work(_ioServices[i].get_executor()));
		//io_context的计数器+1，即使没有异步任务，run也不会退出
	}
	//遍历多个ioservice,创建多个线程，每个线程内部启动ioservice
	for (std::size_t i{}; i < _ioServices.size(); ++i) {
		_threads.emplace_back([this, i]() { //将lambda表达式直接转发给thread的构造函数 启动一个线程 每个线程执行service.run
			_ioServices[i].run();
			});
	}
}
