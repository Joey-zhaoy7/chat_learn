#pragma once
#include <boost/asio.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <queue>
#include <mutex>
#include <memory>
#include "const.h"
#include "MsgNode.h"
//#include "CServer.h"
//#include "LogicSystem.h"


namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>


class CServer;
class LogicSystem;

//管理单个TCP连接的声明周期
//处理信息的接受和发送
//维护会话状态和心跳
//实现网络协议的封装

class CSession : public std::enable_shared_from_this<CSession>
{
public:
	CSession(boost::asio::io_context& io_context, CServer* server);
	~CSession();
	tcp::socket& GetSocket();
	//std::string& GetUuid();
	std::string& GetSessionId();
	void SetUserId(int uid);
	int GetUserId();
	void Start();
	void Send(char* msg, short max_length, short msgid);
	void Send(std::string msg, short msgid);
	void Close();
	std::shared_ptr<CSession> SharedSelf();
	//根据头部中的长度读取完整的消息体 投递到逻辑处理队列
	void AsyncReadBody(int length);
	//读取固定字节 解析信息ID和信息长度 验证合法性
	void AsyncReadHead(int total_len);
	//void NotifyOffline(int uid);
	////判断心跳是否过期
	//bool IsHeartbeatExpired(std::time_t& now);
	////更新心跳
	//void UpdateHeartbeat();
	////处理异常连接
	//void DealExceptionSession();
private:
	//读取完整长度
	void asyncReadFull(std::size_t maxLength, std::function<void(const boost::system::error_code&, std::size_t)> handler);
	//分片读取直到满足长度
	void asyncReadLen(std::size_t  read_len, std::size_t total_len,
		std::function<void(const boost::system::error_code&, std::size_t)> handler);


	void HandleWrite(const boost::system::error_code& error, std::shared_ptr<CSession> shared_self);
	//网络相关
	tcp::socket _socket;
	//std::string _uuid;
	std::string _session_id;//会话唯一标识UUID
	char _data[MAX_LENGTH];//接受缓冲区

	CServer* _server;
	bool _b_close;

	//消息队列管理 发送队列和锁
	std::queue<shared_ptr<SendNode> > _send_que;
	std::mutex _send_lock;

	//收到的消息结构
	std::shared_ptr<RecvNode> _recv_msg_node;
	bool _b_head_parse; //是否正在解析头部
	//收到的头部结构
	std::shared_ptr<MsgNode> _recv_head_node;

	int _user_uid;
	//记录上次接受数据的时间
	//std::atomic<time_t> _last_heartbeat;
	//session 锁
	//std::mutex _session_mtx;
};

class LogicNode {
	friend class LogicSystem;
public:
	LogicNode(shared_ptr<CSession>, shared_ptr<RecvNode>);
private:
	shared_ptr<CSession> _session;
	shared_ptr<RecvNode> _recvnode;
};
