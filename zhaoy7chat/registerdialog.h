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

private:
    void initHttpHandlers();
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
    Ui::RegisterDialog *ui;
    void showTip(QString str, bool b_ok);
};

#endif // REGISTERDIALOG_H
