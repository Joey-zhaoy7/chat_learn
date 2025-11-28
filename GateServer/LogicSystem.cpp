#include "LogicSystem.h"
#include "HttpConnection.h"
#include "VerifyGrpcClient.h"

LogicSystem::LogicSystem()
{
    RegGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
        beast::ostream(connection->_response.body()) << "receive get_test req";
        int i = 0;
        for (auto& elem : connection->_get_params) {
            i++;
            beast::ostream(connection->_response.body())
                << "param" << i << "key is " << elem.first << "vakue is " << elem.second << std::endl;
        }
        });

    RegPost("/get_verifycode", [](std::shared_ptr<HttpConnection> connection) {
        // body transforms to String
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        std::cout << "receive body is <<" << body_str << std::endl;
        connection->_response.set(http::field::content_type, "text/json");
        //Parse Json
        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;
        bool parse_success = reader.parse(body_str, src_root);
        if (!parse_success) {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = ErrorCode::Error_Json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        if (!src_root.isMember("email")) {
            std::cout << "Json does't have the email key" << std::endl;
            root["error"] = ErrorCode::Error_Json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }



        auto email = src_root["email"].asString();
        GetVerifyRsp rsp = VerifyGrpcClient::GetInstance()->GetVerifyCode(email);
        std::cout << "email is " << email << std::endl;
        //root["error"] = 0;
        root["error"] = rsp.error();
        root["email"] = src_root["email"];
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return true;
        });

}

bool LogicSystem::HandleGet(std::string path, std::shared_ptr<HttpConnection> connection)
{
    if (_get_handler.find(path) == _get_handler.end()) {
        return false;
    }
    _get_handler[path](connection);
    return true;
}

bool LogicSystem::HandlePost(std::string path, std::shared_ptr<HttpConnection> connection)
{
    if (_post_handler.find(path) == _post_handler.end()) {
        return false;
    }
    _post_handler[path](connection);
    return true;
}

void LogicSystem::RegGet(std::string url, HttpHandler handler)
{
    _get_handler.insert(make_pair(url, handler));
}

void LogicSystem::RegPost(std::string url, HttpHandler handler)
{
    _post_handler.insert(make_pair(url, handler));
}
