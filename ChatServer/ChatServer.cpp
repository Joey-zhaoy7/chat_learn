// ChatServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <csignal>
#include "ConfigMgr.h"
#include "AsioIOServicePool.h"
#include "CServer.h"

bool bstop = false;
std::condition_variable cond_quit;

int main()
{   
	try {
        auto& cfg = ConfigMgr::Inst();
        auto pool = AsioIOServicePool::GetInstance(); //会创造很多io_context并且绑定work runq起来
        boost::asio::io_context io_context;//主线程
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);

        //为什么参数列表是是两个auto
        //因为async_wait的handler需要两个参数错误码和收到的信号编号

        signals.async_wait([&io_context, pool](auto, auto) {
            io_context.stop();
            pool->Stop();
            });
        auto port_str = cfg["SelfServer"]["Port"];
        CServer s(io_context, atoi(port_str.c_str()));
        io_context.run();//在CServer中绑定了一个监听连接的事件 所以不会退出
    }
    catch(std::exception& e){
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}
