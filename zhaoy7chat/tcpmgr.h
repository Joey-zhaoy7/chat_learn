#ifndef TCPMGR_H
#define TCPMGR_H
#include "global.h"
#include <QTcpSocket>
#include<functional>
#include<QObject>
#include "singleton.h"
#include <QAbstractSocket>


//聊天服务需要TCP长连接，方便服务器和客户端双向通信，那么就需要一个TCPMgr来管理TCP连接
class TcpMgr : public QObject, public Singleton<TcpMgr>, std::enable_shared_from_this<TcpMgr>
{
    Q_OBJECT
public:
    ~TcpMgr();
private:
    TcpMgr();
    friend class Singleton<TcpMgr>;//使得可以访问Singleton的构造
    void initHandlers();//回调处理
    void handleMsg(ReqId id, int len, QByteArray data);

    QMap<ReqId, std::function<void(ReqId, int len, QByteArray data)>> _handlers;
    QTcpSocket _socket;
    QString _host;
    uint16_t _port;
    QByteArray _buffer;
    bool _b_receive_pending;//false->收包完整 true->收包残缺
    quint16 _message_id;//标识
    quint16 _message_len;
signals:
    void sig_con_success(bool bsuccess);
    void sig_send_data(ReqId reqId, QString data);//发送信号给chat server
    void sig_login_failed(int err);
    void sig_switch_chatdlg();
public slots:
    //处理登录界面发送的tcp连接信号
    void slot_tcp_connect(ServerInfo si);
    //进行数据发送
    void slot_send_data(ReqId reqId, QString data);
};

#endif // TCPMGR_H
