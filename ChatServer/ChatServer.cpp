// ChatServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <csignal>
#include "ConfigMgr.h"
#include "AsioIOServicePool.h"
#include "CServer.h"
#include "RedisMgr.h"
#include "ChatServiceImpl.h"

bool bstop = false;
std::condition_variable cond_quit;

int main()
{   
    auto& cfg = ConfigMgr::Inst();
    auto server_name = cfg["SelfServer"]["Name"];
	try {
        //auto& cfg = ConfigMgr::Inst();
        auto pool = AsioIOServicePool::GetInstance(); //会创造很多io_context并且绑定work runq起来
        //将登录数量设置为0
        RedisMgr::GetInstance()->HSet(LOGIN_COUNT, server_name, "0");
        //定义一个GrpcServer，用一个线程启动
        std::string server_address(cfg["SelfServer"]["Host"] + ":" + cfg["SelfServer"]["RPCPort"]);
        ChatServiceImpl service;
        grpc::ServerBuilder builder;
        //监听
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        //构建并启动
        std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
        std::cout << "RPC Server listening on " << server_address << std::endl;
        //用一个下线程启动grpc
        std::thread grpc_server_thread([&server]() {
            server->Wait();
        });

        boost::asio::io_context io_context;//主线程
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);

        //为什么参数列表是是两个auto
        //因为async_wait的handler需要两个参数错误码和收到的信号编号

        signals.async_wait([&io_context, pool,&server](auto, auto) {
            io_context.stop();
            pool->Stop();
            server->Shutdown();
            });

        auto port_str = cfg["SelfServer"]["Port"];
        CServer s(io_context, atoi(port_str.c_str()));
        io_context.run();//在CServer中绑定了一个监听连接的事件 所以不会退出

        //释放资源
        /*auto port_str = cfg["SelfServer"]["Port"];
        CServer s(io_context, atoi(port_str.c_str()));
        io_context.run();*/

        RedisMgr::GetInstance()->HDel(LOGIN_COUNT, server_name);
        RedisMgr::GetInstance()->Close();
        grpc_server_thread.join();
    }
    catch(std::exception& e){
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}
