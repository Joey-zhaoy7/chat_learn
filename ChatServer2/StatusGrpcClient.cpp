#include "StatusGrpcClient.h"
#include "ConfigMgr.h"

StatusConPool::StatusConPool(size_t poolsize, std::string host, std::string port):
	poolSize_(poolsize), host_(host), port_(port), b_stop_(false)
{
	//创建多个连接放入队列
	for (size_t i = 0; i < poolSize_; i++) {
		std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port,
			grpc::InsecureChannelCredentials());
		connections_.push(StatusService::NewStub(channel));//创建stub对象
	}
}

StatusConPool::~StatusConPool()
{
	std::lock_guard<std::mutex> lock(mutex_);
	Close();
	while (!connections_.empty()) {
		connections_.pop();
	}
}

void StatusConPool::Close() {
	b_stop_ = true;
	cond_.notify_all();
}

std::unique_ptr<StatusService::Stub> StatusConPool::getConnection()
{
	std::unique_lock<std::mutex> lock(mutex_);
	cond_.wait(lock, [this]() {
		if (b_stop_) {
			return true;
		}
		return !connections_.empty();
		});
	if (b_stop_) {
		return nullptr;
	}
	auto context = std::move(connections_.front());
	connections_.pop();
	return context;
}
void StatusConPool::returnConnection(std::unique_ptr<StatusService::Stub> context)
{
	std::lock_guard<std::mutex> lock(mutex_);
	if (b_stop_) {
		return;
	}
	connections_.push(std::move(context));
	cond_.notify_one();
}

StatusGrpcClient::~StatusGrpcClient()
{
}

GetChatServerRsp StatusGrpcClient::GetChatServer(int uid)
{
	ClientContext context;
	GetChatServerRsp reply;
	GetChatServerReq request;
	request.set_uid(uid);
	auto stub = pool_->getConnection();
	//调用远程方法 参数：上下文，请求，响应
	//参数固定吗？ 调用不同的方法参数会变化吗？
	//::grpc::Status StatusService::Stub::GetChatServer(::grpc::ClientContext* context, const ::message::GetChatServerReq& request, ::message::GetChatServerRsp* response)
	//Status GetChatServer(ServerContext* context, const GetChatServerReq* request,GetChatServerRsp* reply) override
	Status status = stub->GetChatServer(&context, request, &reply);
	Defer defer([&stub, this]() {
		pool_->returnConnection(std::move(stub));
		});
	if (status.ok()) {
		return reply;
	}
	else {
		reply.set_error(ErrorCodes::RPCFailed);
		return reply;
	}
}

LoginRsp StatusGrpcClient::Login(int uid, std::string token)
{
	ClientContext context;
	LoginRsp reply;
	LoginReq request;
	request.set_uid(uid);
	request.set_token(token);

	auto stub = pool_->getConnection();
	Status status = stub->Login(&context, request, &reply);
	Defer defer([&stub, this]() {
		pool_->returnConnection(std::move(stub));
	});
	if (status.ok()) {
		return reply;
	}
	else {
		reply.set_error(ErrorCodes::RPCFailed);
		return reply;
	}
}


StatusGrpcClient::StatusGrpcClient()
{
	auto& gCfgMgr = ConfigMgr::Inst();
	std::string host = gCfgMgr["StatusServer"]["Host"];
	std::string port = gCfgMgr["StatusServer"]["Port"];
	pool_.reset(new StatusConPool(5, host, port));
}
