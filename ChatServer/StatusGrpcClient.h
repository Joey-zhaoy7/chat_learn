#pragma once
#include "const.h"
#include "Singleton.h"
#include "ConfigMgr.h"
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "message.pb.h"

/*
	Channel: 用于创建Stub对象的通道
	Stub: 客户端存根，用于调用远程服务的方法
	ClientContext: 用于设置RPC调用的上下文信息，如元数据、截止时间等
	Status: 用于表示RPC调用的结果状态
*/
using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::LoginReq;
using message::LoginRsp;
using message::StatusService;



class StatusConPool {
public:
	StatusConPool(size_t poolsize, std::string host, std::string port);
	~StatusConPool();
	void Close();
	std::unique_ptr<StatusService::Stub> getConnection();//Stub对象
	void returnConnection(std::unique_ptr<StatusService::Stub> context);
private:
	std::atomic<bool> b_stop_;
	size_t poolSize_;
	std::string host_;//这里是服务器地址
	std::string port_;//服务器端口
	std::queue<std::unique_ptr<StatusService::Stub>> connections_;
	std::condition_variable cond_;
	std::mutex mutex_;

};

class StatusGrpcClient : public Singleton<StatusGrpcClient>
{
	friend class Singleton<StatusGrpcClient>;
public:
	~StatusGrpcClient();
	//返回获取聊天服务器地址和端口以及token
	GetChatServerRsp GetChatServer(int uid);
	LoginRsp Login(int uid, std::string token);
	//LoginRsp Login(int uid, const std::string& token);

private:
	StatusGrpcClient();
	std::unique_ptr<StatusConPool> pool_;
};

