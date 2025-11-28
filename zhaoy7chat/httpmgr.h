#ifndef HTTPMGR_H
#define HTTPMGR_H
#include "singleton.h"
#include <QString>
#include <QUrl>
#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>

class HttpMgr : public QObject, public Singleton<HttpMgr>, public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT
public:


    /*
     * Singleton返回一个_instance, _instance是一个T类型智能指针，_instance是Singleton的一个静态成员
     * 当Singleton执行析构时，会释放_instance， 调用_instance的析构 = 智能指针的析构 ，智能指针内部要调用T的析构
     * T就是HttpMgr
     * 所以HttpMgr是public
     */

    ~HttpMgr();
    void PostHttpReq(QUrl  url, QJsonObject json, ReqId req_id, Modules mod);
private:
    /*
     * 底层构造子类时要构造基类 构造基类用到了Singleton 所以Singleton构造函数是protected
     * _instance = std::shared_ptr<T>(new T); new HttpMgr, 而HttpMgr的构造函数是私有的，所以需要把基类声明为友元类
     */
    friend class Singleton<HttpMgr>;
    HttpMgr();

    QNetworkAccessManager _manager;
    //定义一个发送的函数
    //参数透传 ReqId模块具体功能，Modules哪个模块


private slots:
    void slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
signals:
    //当一个http发送完之后，会发送这样一个信号通知给其他模块 其他模块做界面的显示之类
    void sig_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
    //http给其他模块发送信息
    void sig_http_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
};

#endif // HTTPMGR_H
