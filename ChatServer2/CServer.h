#pragma once
#include <boost/asio.hpp>
#include "const.h"
#include "CSession.h"
#include <memory.h>
class CServer : public std::enable_shared_from_this<CServer>
{
public:
	//io_context为什么是引用？io_context没有拷贝构造
	CServer(boost::asio::io_context& io_context, short port);
	~CServer();
	//当我们发现Session无效时，从map中清除
	void ClearSession(std::string uuid);
	/*shared_ptr<CSession> GetSession(std::string);
	bool CheckValid(std::string);
	void on_timer(const boost::system::error_code& ec);
	void StartTimer();
	void StopTimer();*/
private:
	boost::asio::io_context& _io_context;//主线程创建的io_context
	boost::asio::ip::tcp::acceptor _acceptor;
	short _port;
	std::map < std::string, std::shared_ptr<CSession>> _sessions;
	std::mutex _mutex;//不同的线程都访问_sessions 所以使用mutex
	//boost::asio::steady_timer _timer;

	void HandleAccept(std::shared_ptr<CSession>, const boost::system::error_code& error);
	void StartAccept();
};

