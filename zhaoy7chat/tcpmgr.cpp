#include "tcpmgr.h"
#include "usermgr.h"
#include "userdata.h"

TcpMgr::~TcpMgr()
{

}

TcpMgr::TcpMgr():
    _host(""), _port(0),_b_receive_pending(false),_message_id(0),_message_len(0)
{
    //连接完成
    connect(&_socket, &QTcpSocket::connected,[&](){//捕获所有
        qDebug() << "Connected to server!";
        //sent info

        //发送信号给登录界面 表示连接成功
        emit sig_con_success(true);
    });

    //有数据可读
    connect(&_socket, &QTcpSocket::readyRead,[&](){
        _buffer.append(_socket.readAll());
        QDataStream stream(&_buffer, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_5_0);
        //
        forever{

            if(!_b_receive_pending){//_b_receive_pending初始化为false，自动进入循环
                if(_buffer.size() < static_cast<int>(sizeof(quint16) * 2)){
                    return; //数据不够完整协议头 id+len
                }
                stream >> _message_id >> _message_len;
                _buffer = _buffer.mid(sizeof(quint16)*2);
                qDebug() <<"Message ID:" <<_message_id <<", Length:" <<_message_len;
            }
            if(_buffer.size() < _message_len){
                _b_receive_pending = true;
                return;
            }
            _b_receive_pending = false;
            //读取信息体
            QByteArray messageBody = _buffer.mid(0,_message_len);
            qDebug() <<"receive body msg is" <<messageBody;
            _buffer = _buffer.mid(_message_len);
            handleMsg(ReqId(_message_id),_message_len, messageBody);
        }
    });

    QObject::connect(&_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), [&](QAbstractSocket::SocketError socketError) {
        Q_UNUSED(socketError)
        qDebug() << "Error:" << _socket.errorString();
    });

    connect(&_socket, &QTcpSocket::disconnected,[&](){
        qDebug() << "DIsconnected from server";
    });
    //写
    connect(this,&TcpMgr::sig_send_data,this,&TcpMgr::slot_send_data);
    initHandlers();

}

void TcpMgr::initHandlers()
{
    //登录回调
    _handlers.insert(ID_CHAT_LOGIN_RSP,[this](ReqId id, int len, QByteArray data){
        Q_UNUSED(len);
        qDebug() << "handle id is   "<< id <<"data is "<<data;
        //将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        //检查是否转换成功
        if(jsonDoc.isNull()){
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        if(!jsonObj.contains("error")){
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Login Failed, err is json parse err" << err;
        }
        int err = jsonObj["error"].toInt();
        if(err != ErrorCodes::SUCCESS){
            qDebug() << "Login Failed, error is "<< err;
            emit sig_login_failed(err);
            return;
        }
        // UserMgr::GetInstance()->SetName(jsonObj["name"].toString());
        // UserMgr::GetInstance()->SetToken(jsonObj["token"].toString());
        // UserMgr::GetInstance()->SetUid(jsonObj["uid"].toInt());
        auto uid = jsonObj["uid"].toInt();
        auto name = jsonObj["name"].toString();
        auto nick = jsonObj["nick"].toString();
        auto icon = jsonObj["icon"].toString();
        auto sex = jsonObj["sex"].toInt();
        auto desc = jsonObj["desc"].toString();
        auto user_info = std::make_shared<UserInfo>(uid, name, nick, icon, sex);

        UserMgr::GetInstance()->SetUserInfo(user_info);
        UserMgr::GetInstance()->SetToken(jsonObj["token"].toString());
        if(jsonObj.contains("apply_list")){
            UserMgr::GetInstance()->AppendApplyList(jsonObj["apply_list"].toArray());
        }

        //添加好友列表
        if (jsonObj.contains("friend_list")) {
            UserMgr::GetInstance()->AppendFriendList(jsonObj["friend_list"].toArray());
        }


        emit sig_switch_chatdlg();
    });

    //搜索用户回调
    _handlers.insert(ID_SEARCH_USER_RSP,[this](ReqId id, int len, QByteArray data){
        Q_UNUSED(len);
        qDebug() << "handle id is   "<< id <<"data is "<<data;
        //将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        //检查是否转换成功
        if(jsonDoc.isNull()){
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        if(!jsonObj.contains("error")){
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "err is json parse err" << err;
            emit sig_user_search(nullptr);
        }
        int err = jsonObj["error"].toInt();
        if(err != ErrorCodes::SUCCESS){
            qDebug() << "Errcodes is not success "<< err;
            emit sig_user_search(nullptr);
            return;
        }
        //根据json封装对象类
        auto search_info =  std::make_shared<SearchInfo>(jsonObj["uid"].toInt(), jsonObj["name"].toString(),
                                                        jsonObj["nick"].toString(), jsonObj["desc"].toString(),
                                                        jsonObj["sex"].toInt(), jsonObj["icon"].toString());
        emit sig_user_search(search_info);
    });

    //点击添加好友，服务端处理完后发送ID_ADD_FRIEND_RSP回调 下面客户端进行处理
    _handlers.insert(ID_ADD_FRIEND_RSP,[this](ReqId id, int len, QByteArray data){
        Q_UNUSED(len);
        qDebug() << "handle id is   "<< id <<"data is "<<data;
        //将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        //检查是否转换成功
        if(jsonDoc.isNull()){
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        if(!jsonObj.contains("error")){
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "ID_ADD_FRIEND_RSP err is json parse err" << err;
            return;
        }
        int err = jsonObj["error"].toInt();
        if(err != ErrorCodes::SUCCESS){
            qDebug() << "ID_ADD_FRIEND_RSP Errcodes is not success "<< err;
            return;
        }
        qDebug() << "ID_ADD_FRIEND_RSP success ";

    });

    //通知
    _handlers.insert(ID_NOTIFY_ADD_FRIEND_REQ,[this](ReqId id, int len, QByteArray data){
        Q_UNUSED(len);
        qDebug() << "handle id is   "<< id <<"data is "<<data;
        //将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        //检查是否转换成功
        if(jsonDoc.isNull()){
            qDebug() << " Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        if(!jsonObj.contains("error")){
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "ID_NOTIFY_ADD_FRIEND_REQ err is json parse err" << err;
        }
        int err = jsonObj["error"].toInt();
        if(err != ErrorCodes::SUCCESS){
            qDebug() << "ID_ADD_FRIEND_RSP Errcodes is not success "<< err;
            return;
        }
        int from_uid = jsonObj["applyuid"].toInt();
        QString name = jsonObj["name"].toString();
        QString desc = jsonObj["desc"].toString();
        QString icon = jsonObj["icon"].toString();
        QString nick = jsonObj["nick"].toString();
        int sex = jsonObj["sex"].toInt();

        auto apply_info = std::make_shared<AddFriendApply>(
            from_uid, name, desc,
            icon, nick, sex);

        emit sig_friend_apply(apply_info);
        qDebug() << "ID_NOTIFY_ADD_FRIEND_REQ success ";
    });

    //
    _handlers.insert(ID_NOTIFY_AUTH_FRIEND_REQ, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Auth Friend Failed, err is " << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Auth Friend Failed, err is " << err;
            return;
        }

        int from_uid = jsonObj["fromuid"].toInt();
        QString name = jsonObj["name"].toString();
        QString nick = jsonObj["nick"].toString();
        QString icon = jsonObj["icon"].toString();
        int sex = jsonObj["sex"].toInt();

        auto auth_info = std::make_shared<AuthInfo>(from_uid,name,
                                                    nick, icon, sex);

        emit sig_add_auth_friend(auth_info);
    });

    //
    _handlers.insert(ID_AUTH_FRIEND_RSP, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Auth Friend Failed, err is Json Parse Err" << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Auth Friend Failed, err is " << err;
            return;
        }

        auto name = jsonObj["name"].toString();
        auto nick = jsonObj["nick"].toString();
        auto icon = jsonObj["icon"].toString();
        auto sex = jsonObj["sex"].toInt();
        auto uid = jsonObj["uid"].toInt();
        auto rsp = std::make_shared<AuthRsp>(uid, name, nick, icon, sex);
        emit sig_auth_rsp(rsp);

        qDebug() << "Auth Friend Success " ;
    });

    _handlers.insert(ID_TEXT_CHAT_MSG_RSP, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Chat Msg Rsp Failed, err is Json Parse Err" << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Chat Msg Rsp Failed, err is " << err;
            return;
        }

        qDebug() << "Receive Text Chat Rsp Success " ;
        //ui设置送达等标记 todo...
    });

    _handlers.insert(ID_NOTIFY_TEXT_CHAT_MSG_REQ, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Notify Chat Msg Failed, err is Json Parse Err" << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Notify Chat Msg Failed, err is " << err;
            return;
        }

        qDebug() << "Receive Text Chat Notify Success " ;
        auto msg_ptr = std::make_shared<TextChatMsg>(jsonObj["fromuid"].toInt(),
                                                     jsonObj["touid"].toInt(),jsonObj["text_array"].toArray());
        emit sig_text_chat_msg(msg_ptr);
    });

}

void TcpMgr::handleMsg(ReqId id, int len, QByteArray data)
{
    auto find_iter = _handlers.find(id);
    if(find_iter == _handlers.end()){
        qDebug() << "not found id ["<< id<<"] to handle";
    }
    find_iter.value()(id,len,data);
}




void TcpMgr::slot_tcp_connect(ServerInfo si)
{
    //连接服务器
    qDebug()<<" receive tcp connect signal";
    qDebug()<<"Connecting to server...";
    _host = si.Host;
    _port=static_cast<uint16_t>(si.Port.toUInt());
    //异步非阻塞。连接完成会发送connected, disconnected,errorOccurred,stateChanged信号
    qDebug() << "host: "<<_host <<" port: "<< _port;
    _socket.connectToHost(_host, _port);
}

void TcpMgr::slot_send_data(ReqId reqId, QByteArray dataBytes)
{
    uint16_t id = reqId;
    // QByteArray dataBytes = data.toUtf8();
    quint16 len = static_cast<quint16>(dataBytes.size());

    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out << id << len;
    block.append(dataBytes);

    _socket.write(block);
    // 实际发生的时间：
    // 1. 立即：数据放入 Qt 内部缓冲区
    // 2. 稍后：Qt 事件循环调用操作系统 send()
    // 3. 异步：操作系统根据 TCP 协议决定何时真正发送

}
