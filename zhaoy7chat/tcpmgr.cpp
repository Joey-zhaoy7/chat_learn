#include "tcpmgr.h"



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

}

void TcpMgr::slot_tcp_connect(ServerInfo si)
{
    //连接服务器
    qDebug()<<" receive tcp connect signal";
    qDebug()<<"Connecting to server...";
    _host = si.Host;
    _port=static_cast<uint16_t>(si.Port.toUInt());
    //异步非阻塞。连接完成会发送connected, disconnected,errorOccurred,stateChanged信号
    _socket.connectToHost(_host, _port);
}

void TcpMgr::slot_send_data(ReqId reqId, QString data)
{
    uint16_t id = reqId;
    QByteArray dataBytes = data.toUtf8();
    quint16 len = static_cast<quint16>(data.size());

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
