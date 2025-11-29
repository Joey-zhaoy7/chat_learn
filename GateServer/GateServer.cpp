#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "CServer.h"
#include "ConfigMgr.h"

// http://localhost:8080/get_test
int main() {
    //ConfigMgr gCfgMgr;
    ConfigMgr& gCfgMgr = ConfigMgr::Inst();
    std::string gate_port_str = gCfgMgr["GateServer"]["port"];
    unsigned short gate_port = atoi(gate_port_str.c_str());

    try
    {
        unsigned short port = static_cast<unsigned short>(8080);
        net::io_context ioc{ 1 };
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);//捕获两个信号以终止服务器
        signals.async_wait([&ioc](const boost::system::error_code& error, int signal_number) {
            if (error) {
                return;
            }
            ioc.stop();//事件停止循环
            });
        std::make_shared<CServer>(ioc, port)->Start();
        std::cout << "Gate Server listen on port: " << port << std::endl;
        ioc.run();//运行事件循环
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error is" << e.what() << '\n';
        return EXIT_FAILURE;
    }

}