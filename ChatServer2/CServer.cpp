#include "CServer.h"
#include "AsioIOServicePool.h"
#include "CSession.h"
#include "ConfigMgr.h"
#include "RedisMgr.h"
#include "UserMgr.h"
CServer::CServer(boost::asio::io_context& io_context, short port):
	_io_context(io_context),//主函数的服务
	_port(port),
	_acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
	//_timer(_io_context, std::chrono::seconds(60))
{
	std::cout << "Server start success, listen on port:" << _port << std::endl;
	StartAccept();
}

CServer::~CServer()
{
	std::cout << "Server destruct listen on port: " << _port << std::endl;
}

void CServer::ClearSession(std::string session_id)
{
	if (_sessions.find(session_id) != _sessions.end()) {
		//remove link between uid and session
		UserMgr::GetInstance()->RmvUserSession(_sessions[session_id]->GetUserId());
	}

	{
		lock_guard<mutex> lock(_mutex);
		_sessions.erase(session_id);
	}
}

//bool CServer::CheckValid(std::string uuid)
//{
//	lock_guard<mutex> lock(_mutex);
//	auto it = _sessions.find(uuid);
//	if (it != _sessions.end()) {
//		return true;
//	}
//	return false;
//}

//void CServer::on_timer(const boost::system::error_code& ec) {
//	if (ec) {
//		std::cout << "timer error: " << ec.message() << std::endl;
//		return;
//	}
//	std::vector<std::shared_ptr<CSession>> _expired_sessions;
//	int session_count = 0;
//	//此处加锁遍历session
//	std::map<std::string, shared_ptr<CSession>> sessions_copy;
//	{
//		lock_guard<mutex> lock(_mutex);
//		sessions_copy = _sessions;
//	}
//
//	time_t now = std::time(nullptr);
//	for (auto iter = sessions_copy.begin(); iter != sessions_copy.end(); iter++) {
//		auto b_expired = iter->second->IsHeartbeatExpired(now);
//		if (b_expired) {
//			//关闭socket, 其实这里也会触发async_read的错误处理
//			iter->second->Close();
//			//收集过期信息
//			_expired_sessions.push_back(iter->second);
//			continue;
//		}
//		session_count++;
//	}
//
//	//设置session数量
//	auto& cfg = ConfigMgr::Inst();
//	auto self_name = cfg["SelfServer"]["Name"];
//	auto count_str = std::to_string(session_count);
//	RedisMgr::GetInstance()->HSet(LOGIN_COUNT, self_name, count_str);//将server中存在多少个有效的session存入redis
//
//	//处理过期session, 单独提出，防止死锁
//	for (auto& session : _expired_sessions) {
//		session->DealExceptionSession();
//	}
//
//	//再次设置，下一个60s检测
//	_timer.expires_after(std::chrono::seconds(60));
//	_timer.async_wait([this](boost::system::error_code ec) {
//		on_timer(ec);
//		});
//}

//void CServer::StartTimer()
//{
//	//启动定时器
//	auto self(shared_from_this());
//	_timer.async_wait([self](boost::system::error_code ec) {
//		self->on_timer(ec);
//		});
//}
//
//void CServer::StopTimer()
//{
//	_timer.cancel();
//}


void CServer::HandleAccept(std::shared_ptr<CSession>new_session, const boost::system::error_code& error)
{
	if (!error) {
		new_session->Start(); //开始读取
		lock_guard<mutex> lock(_mutex);
		_sessions.insert(std::make_pair(new_session->GetSessionId(), new_session));
	}
	else
	{
		std::cout << "sesstion accept failed, error is" << error.what() << std::endl;
	}
	StartAccept();//继续监听下一个连接
}

void CServer::StartAccept()
{
	//取出一个服务，绑定新的session用
	auto& _io_context = AsioIOServicePool::GetInstance()->GetIOService();
	std::shared_ptr<CSession> new_sesstion = make_shared<CSession>(_io_context, this);

	// acceptor在主线程的io_context上异步等待  有客户端连接时触发async_accept回调 执行handleAccept
	_acceptor.async_accept(new_sesstion->GetSocket(), std::bind(&CServer::HandleAccept, this, new_sesstion, placeholders::_1));
	//等效写法
	/*_acceptor.async_accept(
		new_sesstion->GetSocket(),
		[this, new_sesstion](boost::system::error_code error) {
		this->HandleAccept(new_sesstion, error);
	}
	);*/
}


