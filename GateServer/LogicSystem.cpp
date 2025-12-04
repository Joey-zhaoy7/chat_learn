#include "LogicSystem.h"
#include "HttpConnection.h"
#include "VerifyGrpcClient.h"
#include  "RedisMgr.h"
#include "MysqlMgr.h"

LogicSystem::LogicSystem()
{
    RegPost("/user_register", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        std::cout << "user_register receive body is   " << body_str << std::endl;
        //write response
        connection->_response.set(http::field::content_type, "text/json");
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

        auto email = src_root["email"].asString();
        auto name = src_root["user"].asString();
        auto pwd = src_root["passwd"].asString();
        auto confirm = src_root["confirm"].asString();

        if (pwd != confirm) {
            std::cout << "passwd is different" << std::endl;
            root["error"] = ErrorCode::PasswdError;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        //check the verifycode
        std::string verify_code;
        bool b_get_verify = RedisMgr::GetInstance()->Get(CODEPREFIX+src_root["email"].asString(), verify_code);
        if (!b_get_verify) {
            std::cout << "get verify code expired" << std::endl;
            root["error"] = ErrorCode::VerifyCodeExpired;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }
        if (verify_code != src_root["verifycode"].asString()) {
            std::cout << "verify code error" << std::endl;
            root["error"] = ErrorCode::VerifyCodeError;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }
        
		int uid = MysqlMgr::GetInstance()->RegUser(name, email, pwd);
        if (uid == 0 || uid == -1) {
			std::cout << "register user failed, user exist or sql error" << std::endl;
			root["error"] = ErrorCode::UserExist;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        //查找数据库判断用户是否存在
        root["error"] = 0;
        root["email"] = email;
		root["uid"] = uid;
        root["user"] = name;
        root["passwd"] = pwd;
        root["confirm"] = confirm;

        root["verifycode"] = src_root["verifycode"].asString();
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return true;




        });
        

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
