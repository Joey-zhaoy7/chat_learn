#ifndef GLOBAL_H
#define GLOBAL_H
#include <QWidget>
#include <functional>
#include "QStyle"
#include "QByteArray"
#include <QJsonObject>
#include <QRegularExpression>
#include <QNetworkReply>
#include <QDir>
#include <QSettings>
#include  <memory>
#include <iostream>
#include <mutex>

extern std::function<void(QWidget*)> repolish; //声明

//定义透传
enum ReqId{
    ID_GET_VERIFY_CODE = 1001,//获取验证码
    ID_REG_USER = 1002, //注册用户
};

enum Modules{
    REGISTERMOD = 0,
};

enum ErrorCodes{
    SUCCESS = 0,
    ERR_JSON = 1,
    ERR_NETWORK = 2,
};

extern QString gate_url_prefix;

#endif // GLOBAL_H
