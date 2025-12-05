#ifndef GLOBAL_H
#define GLOBAL_H
#include <QWidget>
#include <QMouseEvent>
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
//声明 function<return-type(arg-type1, arg-type2, etc.)>
extern std::function<void(QWidget*)> repolish;
extern std::function<QString(QString)> xorString;

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

enum TipErr{
    TIP_SUCCESS = 0,
    TIP_EMAIL_ERR = 1,
    TIP_PWD_ERR = 2,
    TIP_CONFIRM_ERR = 3,
    TIP_PWD_CONFIRM = 4,
    TIP_VARIFY_ERR = 5,
    TIP_USER_ERR = 6
};

enum ClickLbState{
    Normal = 0,
    Selected = 1
};

extern QString gate_url_prefix;

#endif // GLOBAL_H
