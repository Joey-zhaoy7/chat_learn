#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H
#include "global.h"
#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private:
    Ui::LoginDialog *ui;
    void initHeader();
    bool checkEmailValid();
    bool checkPwdValid();
    bool enableBtn(bool enabled);

    void showTip(QString str, bool b_ok);
    void AddTipErr(TipErr te, QString tips);
    void DelTipErr(TipErr te);
    QMap<TipErr, QString> tip_err_;

    //处理回调
    void initHttpHandlers();
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
    int _uid;
    QString _token;

public slots:
    void slot_forget_pwd();
signals:
    void switchRegister();
    void switchReset();
    void sig_connect_tcp(ServerInfo);// 发送一个信号给tcpMgr，tcpMgr发送一个强连接
private slots:
    void on_login_btn_clicked();
    void slot_http_login_mod_finish(ReqId id, QString res, ErrorCodes err);
};

#endif // LOGINDIALOG_H
