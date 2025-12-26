#ifndef TCPMGR_H
#define TCPMGR_H
#include <QTcpSocket>
#include<functional>
#include<QObject>
#include <QAbstractSocket>
#include "userdata.h"
#include "global.h"
#include "singleton.h"
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
    void sig_send_data(ReqId reqId, QByteArray data);//发送信号给chat server
    void sig_login_failed(int err);
    void sig_switch_chatdlg();
    //搜索用户
    void sig_user_search(std::shared_ptr<SearchInfo> si);
    //同意添加好友，服务器的回包
    void sig_auth_rsp(std::shared_ptr<AuthRsp>);
    //收到对方好友申请
    void sig_friend_apply(std::shared_ptr<AddFriendApply>);
    //我收到对方同意的认证 对方同意了发送这个信号
    void sig_add_auth_friend(std::shared_ptr<AuthInfo>);

    void sig_text_chat_msg(std::shared_ptr<TextChatMsg> msg);
public slots:
    //处理登录界面发送的tcp连接信号
    void slot_tcp_connect(ServerInfo si);
    //进行数据发送
    void slot_send_data(ReqId reqId, QByteArray data);
};

#endif // TCPMGR_H
