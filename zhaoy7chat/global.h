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

//定义聊天界面的模式
enum ChatUIMode{
    SearchMode,
    ChatMode,
    ContactMode,
};

//定义透传
enum ReqId{
    ID_GET_VERIFY_CODE = 1001,//获取验证码
    ID_REG_USER = 1002, //注册用户
    ID_RESET_PWD = 1003,//修改密码
    ID_LOGIN_USER = 1004,
    ID_CHAT_LOGIN =1005,
    ID_CHAT_LOGIN_RSP = 1006,//登录聊天界面回包
};

enum Modules{
    REGISTERMOD = 0,
    RESETMOD = 1,
    LOGINMODE = 2,
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

enum ListItemType{
    CHAT_USER_ITEM,
    CONTACT_USER_ITEM,
    SEARCH_USER_ITEM,
    ADD_USER_TIP_ITEM,
    INVALID_ITEM,//不可点击条目
    GROUP_TIP_ITEM,//分组提示条目
};

enum ChatRole{
    Self,
    Other

};

struct MsgInfo{
    QString msgFlag;//"text,image,file"
    QString content;//表示文件和图像的url,文本信息
    QPixmap pixmap;//文件和图片的缩略图
};

extern QString gate_url_prefix;

//tcp
struct ServerInfo{
    QString Host;
    QString Port;
    QString Token;
    int Uid;
};

#endif // GLOBAL_H
