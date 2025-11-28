#include "CServer.h"
#include "HttpConnection.h"
CServer::CServer(boost::asio::io_context& ioc, unsigned short& port)
    :_ioc(ioc), _acceptor(ioc, tcp::endpoint(tcp::v4(), port)), _socket(ioc) {
    //监听函数
}


//用于监听新链接
void CServer::Start() {
    auto self = shared_from_this();
    _acceptor.async_accept(_socket, [self](beast::error_code ec) {
        try
        {
            //如果出错放弃这个链接，继续监听其他链接
            if (ec) {
                self->Start();
                return;

            }
            //想让socket复用，但是socket是没有复制构造的,要么移动，要么引用，引用是会出问题的，_acceptor会把新数据写入socket中
            //create new connection and create HttpConnection to manager this connection.
            std::make_shared<HttpConnection>(std::move(self->_socket))->Start();

            //keep listening
            self->Start();
        }
        catch (std::exception& exp)
        {
            std::cerr << exp.what() << '\n';
        }

        });
};