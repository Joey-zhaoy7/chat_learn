#include "httpmgr.h"

HttpMgr::~HttpMgr()
{

}

HttpMgr::HttpMgr() {
    connect(this, &HttpMgr::sig_http_finish, this, &HttpMgr::slot_http_finish);
}

void HttpMgr::PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod)
{
    //data serivalized to string
    QByteArray data = QJsonDocument(json).toJson();

    //url request
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));
    auto self = shared_from_this();

    //reply    reply需要自己回收
    QNetworkReply* reply = _manager.post(request, data);
    QObject::connect(reply, &QNetworkReply::finished,[self, reply, req_id, mod](){
        if(reply ->error() != QNetworkReply::NoError){
            qDebug() << reply->errorString();
            emit self->sig_http_finish(req_id,"",ErrorCodes::ERR_NETWORK, mod);
            reply->deleteLater();
            return;
        }
        QString res = reply->readAll();
        //无论成功还是失败，都会发这个信号
        emit self->sig_http_finish(req_id, res, ErrorCodes::SUCCESS, mod);
        reply->deleteLater();
        return;
    });
}

void HttpMgr::slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod)
{
    if(mod == Modules::REGISTERMOD){
        emit sig_http_reg_mod_finish(id, res, err);
    }
}
