#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "httpmgr.h"
#include "global.h"
RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    ui->pass_edit->setEchoMode(QLineEdit::Password);
    ui->confirm_edit->setEchoMode(QLineEdit::Password);

    //实现刷新
    ui->err_tip->setProperty("state", "normal");
    repolish(ui->err_tip);

    //connect
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_http_reg_mod_finish, this, &RegisterDialog::slot_reg_mod_finish);

    initHttpHandlers();
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_get_code_btn_clicked()
{
    //验证邮箱的地址正则表达式
    auto email = ui->emai_edit->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(match){
        //发送http请求获取验证码
        QJsonObject json_obj;
        json_obj["email"] = email;
        HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/get_verifycode"),
                                            json_obj,ReqId::ID_GET_VERIFY_CODE, Modules::REGISTERMOD);
        // showTip(tr("test"),false);
    }else{
        //提示邮箱不正确
        showTip(tr("邮箱地址不正确"),false);
    }
}

void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS){
        showTip(tr("网络请求错误"),false);
        return;
    }
    //parse json to ByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson((res.toUtf8()));
    if(jsonDoc.isEmpty()){
        showTip(tr("json parse failure"), false);
        return;
    }
    if(!jsonDoc.isObject()){
        showTip(tr("json parse failure"), false);
        return;
    }
    jsonDoc.object();//json对象传给回调函数 让回调函数进行处理
    _handlers[id](jsonDoc.object());
    return;
}

void RegisterDialog::initHttpHandlers()
{
    _handlers.insert(ReqId::ID_GET_VERIFY_CODE, [this](const QJsonObject& jsonObject){
        //确保接收服务器返回的json中 含error  email两个key
        int error = jsonObject["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("parameters error"), false);
            return;
        }
        auto email = jsonObject["email"].toString();
        showTip(tr("email has been sent!"), true);
        qDebug() << "email is " << email;
    });
}


void RegisterDialog::showTip(QString str, bool b_ok)
{
    if(b_ok){
        ui->err_tip->setProperty("state", "normal");
    }else{
        ui->err_tip->setProperty("state", "err");
    }
    ui->err_tip->setText(str);
    repolish(ui->err_tip);
}

