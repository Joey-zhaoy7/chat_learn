#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    connect(ui->register_btn, &QPushButton::clicked, this, &LoginDialog::switchRegister);

    //实现忘记密码功能
    ui->forget_label->SetState("normal","hover","","selected","selected_hover","");

    connect(ui->forget_label,&ClickedLabel::clicked,this, &LoginDialog::slot_forget_pwd);
}

LoginDialog::~LoginDialog()
{
    qDebug() << "destruct LoginDlg";
    delete ui;
}

void LoginDialog::slot_forget_pwd()
{
    qDebug()<<"slot forget pwd";
    emit switchReset();
}
