#pragma once
#include "const.h"
class HttpConnection :public std::enable_shared_from_this<HttpConnection> {
public:
    friend class LogicSystem;
    HttpConnection(tcp::socket socket);
    void Start();//listen context
private:
    void CheckDeadline();
    void WriteResponse();
    void HandleReq();

    tcp::socket _socket;
    beast::flat_buffer _buffer{ 8192 };//接受对方发送过来的数据
    http::request<http::dynamic_body> _request;
    http::response<http::dynamic_body> _response;
    net::steady_timer deadline_{
        _socket.get_executor(), std::chrono::seconds(60)
    };

    //parse
    void PreParseGetParam();
    std::string _get_url;
    std::unordered_map<std::string, std::string> _get_params;


};