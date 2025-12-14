#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "httpmgr.h"
RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog), countdown_(5)
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

    ui->err_tip->clear();

    connect(ui->user_edit,&QLineEdit::editingFinished,this,[this](){
        checkUserValid();
    });

    connect(ui->email_edit, &QLineEdit::editingFinished, this, [this](){
        checkEmailValid();
    });

    connect(ui->pass_edit, &QLineEdit::editingFinished, this, [this](){
        checkPassValid();
    });

    connect(ui->confirm_edit, &QLineEdit::editingFinished, this, [this](){
        checkConfirmValid();
    });

    connect(ui->verifycode_edit, &QLineEdit::editingFinished, this, [this](){
        checkVerifyValid();
    });

    //点击是否显示明文显示密码connect
    connect(ui->pass_visible,&ClickedLabel::clicked, this, [this](){
        auto state = ui->pass_visible->GetCurState();
        if(state == ClickLbState::Normal){
            ui->pass_edit->setEchoMode(QLineEdit::Password);
        }else{
            ui->pass_edit->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "passwd Label was clicked";
    });

    connect(ui->confirm_visible,&ClickedLabel::clicked, this, [this](){
        auto state = ui->confirm_visible->GetCurState();
        if(state == ClickLbState::Normal){
            ui->confirm_edit->setEchoMode(QLineEdit::Password);
        }else{
            ui->confirm_edit->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "confirm Label was clicked";
    });

    //password and confirm password visible
    ui->pass_visible->setCursor(Qt::PointingHandCursor);
    ui->confirm_visible->setCursor(Qt::PointingHandCursor);

    ui->pass_visible->SetState("unvisible", "unvisible_hover", "visible_hover", "visible", "visible_hover", "unvisible_hover");
    ui->confirm_visible->SetState("unvisible", "unvisible_hover", "visible_hover", "visible", "visible_hover", "unvisible_hover");

    //create timer for login ui
    countdown_timer_ = new QTimer(this);
    connect(countdown_timer_, &QTimer::timeout, [this](){
        if(countdown_ == 0){
            countdown_timer_->stop();
            emit sigSwitchLogin();
            return;
        }
        countdown_--;
        auto str = QString("注册成功，%1 s后返回登录").arg(countdown_);
        ui->tip_label->setText(str);
    });
}


RegisterDialog::~RegisterDialog()
{
    qDebug() << "destruct RegDialog";
    delete ui;
}


void RegisterDialog::on_get_code_btn_clicked()
{
    //验证邮箱的地址正则表达式
    auto email = ui->email_edit->text();
    // auto email = "2631621675@qq.com";
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
        qDebug() << "get verify code error is" <<error;
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("parameters error"), false);
            return;
        }
        auto email = jsonObject["email"].toString();
        showTip(tr("email has been sent!"), true);
        qDebug() << "email is " << email;
    });
    _handlers.insert(ReqId::ID_REG_USER,[this](const QJsonObject& jsonObject){
        int error = jsonObject["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip("parameters error",false);
            return;
        }
        auto email = jsonObject["email"].toString();
        showTip(tr("registe success"),true);
        qDebug()<<"user uid is "<<jsonObject["uid"].toString();
        qDebug() << "email is" <<email;
        ChangeTipPage();
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

void RegisterDialog::ChangeTipPage()
{
    countdown_timer_->stop();
    ui->stackedWidget->setCurrentWidget(ui->page_2);

    //启动一个定时器 1s后 发出timeout信号
    countdown_timer_->start(1000);
}
void RegisterDialog::AddTipErr(TipErr te, QString tips){
    tip_err_[te] = tips;
    showTip(tips,false);
}
void RegisterDialog::DelTipErr(TipErr te){
    tip_err_.remove(te);
    if(tip_err_.empty()){
        ui->err_tip->clear();
        return;
    }
    showTip(tip_err_.first(),false);
}

bool RegisterDialog::checkUserValid()
{
    if(ui->user_edit->text() ==""){
        AddTipErr(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_USER_ERR);
    return true;
}

bool RegisterDialog::checkEmailValid()
{
    //验证邮箱的地址正则表达式
    auto email = ui->email_edit->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(!match){
        //提示邮箱不正确
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        return false;
    }

    DelTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool RegisterDialog::checkPassValid()
{
    auto pass = ui->pass_edit->text();

    if(pass.length() < 6 || pass.length()>15){
        //提示长度不准确
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码长度应为6~15"));
        return false;
    }

    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*]{6,15}$");
    bool match = regExp.match(pass).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_PWD_ERR, tr("不能包含非法字符"));
        return false;;
    }

    DelTipErr(TipErr::TIP_PWD_ERR);

    return true;
}

bool RegisterDialog::checkConfirmValid()
{
    auto pass = ui->pass_edit->text();
    auto confirm = ui->confirm_edit->text();
    if(confirm.length() < 6 || confirm.length() >15){
        AddTipErr(TipErr::TIP_CONFIRM_ERR, tr("密码长度应为6-15位"));
        return false;
    }

    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*]{6,15}$");
    bool match = regExp.match(pass).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_CONFIRM_ERR, tr("不能包含非法字符"));
        return false;;
    }
    DelTipErr(TipErr::TIP_PWD_ERR);

    if(pass != confirm){
        AddTipErr(TipErr::TIP_CONFIRM_ERR, tr("确认密码和密码不相同"));
        return false;
    }else{
        DelTipErr(TipErr::TIP_CONFIRM_ERR);
    }

    return true;


}

bool RegisterDialog::checkVerifyValid()
{
    auto pass = ui->verifycode_edit->text();
    if(pass.isEmpty()){
        AddTipErr(TipErr::TIP_VARIFY_ERR, tr("验证码不能为空"));
        return false;
    }

    DelTipErr(TipErr::TIP_VARIFY_ERR);
    return true;
}


void RegisterDialog::on_sure_btn_clicked()
{
    if(ui->user_edit->text() == ""){
        showTip(tr("用户名不能为空"), false);
        return;
    }

    if(ui->email_edit->text() == ""){
        showTip(tr("邮箱不能为空"), false);
        return;
    }

    if(ui->pass_edit->text() == ""){
        showTip(tr("密码不能为空"), false);
        return;
    }

    if(ui->confirm_edit->text() == ""){
        showTip(tr("确认密码不能为空"), false);
        return;
    }

    if(ui->confirm_edit->text() != ui->pass_edit->text()){
        showTip(tr("密码和确认密码不匹配"), false);
        return;
    }

    if(ui->verifycode_edit->text() == ""){
        showTip(tr("验证码不能为空"), false);
        return;
    }

    //send http request
    QJsonObject json_obj;
    json_obj["user"] = ui->user_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["passwd"] =xorString(ui->pass_edit->text());
    json_obj["confirm"] = xorString(ui->confirm_edit->text());

    // json_obj["user"] = "user";
    // json_obj["email"] = "2631621675@qq.com";
    // json_obj["passwd"] = "password";
    // json_obj["confirm"] = "password";

    json_obj["verifycode"] = ui->verifycode_edit->text();
    //void PostHttpReq(QUrl  url, QJsonObject json, ReqId req_id, Modules mod);
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/user_register"),json_obj,ReqId::ID_REG_USER, Modules::REGISTERMOD);
}

void RegisterDialog::on_return_btn_clicked()
{
    countdown_timer_->stop();
    emit sigSwitchLogin();
}


void RegisterDialog::on_cancel_btn_clicked()
{
    countdown_timer_->stop();
    emit sigSwitchLogin();
}

