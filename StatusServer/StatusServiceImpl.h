#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include <mutex>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::LoginReq;
using message::LoginRsp;
using message::StatusService;

//定义一个简单的ChatServer类 用于存储聊天服务器的信息
class  ChatServer {
public:
	ChatServer():host(""),port(""),name(""),con_count(0){}
	ChatServer(const ChatServer& cs):host(cs.host), port(cs.port), name(cs.name), con_count(cs.con_count){}
	ChatServer& operator=(const ChatServer& cs) {
		if (&cs == this) {
			return *this;
		}

		host = cs.host;
		name = cs.name;
		port = cs.port;
		con_count = cs.con_count;
		return *this;
	}
	std::string host;
	std::string port;
	std::string name;
	int con_count;
};

//final关键字表示该类不能被继承 
// StatusService 是由Protobuf根据.proto文件生成的服务基类 Service是其内部定义的抽象类，包含了RPC方法的纯虚函数声明

class StatusServiceImpl final : public StatusService::Service
{
public:
	StatusServiceImpl();
	//GetChatServer是StatusService服务中的一个RPC方法
	Status GetChatServer(ServerContext* context, const GetChatServerReq* request,
		GetChatServerRsp* reply) override;
	Status Login(ServerContext* context, const LoginReq* request,
		LoginRsp* reply) override;
private:
	void insertToken(int uid, std::string token);
	ChatServer getChatServer();
	//为什么是unordered_map? 因为查找效率高 而普通map是红黑树实现的 查找效率低
	//为什么连接池是使用queue? 因为连接池是先进先出
	std::unordered_map<std::string, ChatServer> _servers;
	std::mutex _server_mtx;

};

