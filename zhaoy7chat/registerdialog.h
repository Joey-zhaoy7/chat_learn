#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include "global.h"
#include <QDialog>

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    //点击获取验证码槽函数
    void on_get_code_btn_clicked();

    //http请求结束槽函数
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);

    //注册界面的确认按钮槽函数
    void on_sure_btn_clicked();

    //注册成功界面
    void on_return_btn_clicked();

private:
    void initHttpHandlers();
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
    Ui::RegisterDialog *ui;

    //tip
    void AddTipErr(TipErr te, QString tips);
    void DelTipErr(TipErr te);
    void showTip(QString str, bool b_ok);
    void ChangeTipPage();
    // std::map<TipErr> tip_err_;
    QMap<TipErr, QString> tip_err_;

    //check valid
    bool checkUserValid();
    bool checkEmailValid();
    bool checkPassValid();
    bool checkConfirmValid();
    bool checkVerifyValid();

    //定时器
    QTimer* countdown_timer_;
    int countdown_;
signals:
    //注册成功，返回登录界面
    void sigSwitchLogin();
};

#endif // REGISTERDIALOG_H
