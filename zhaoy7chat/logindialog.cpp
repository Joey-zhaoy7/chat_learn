#include "logindialog.h"
#include "ui_logindialog.h"
#include "httpmgr.h"
#include <QPainter>
#include <QPainterPath>
#include "tcpmgr.h"
LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    connect(ui->register_btn, &QPushButton::clicked, this, &LoginDialog::switchRegister);

    //实现忘记密码功能
    ui->forget_label->SetState("normal","hover","","selected","selected_hover","");

    connect(ui->forget_label,&ClickedLabel::clicked,this, &LoginDialog::slot_forget_pwd);
    initHeader();
    initHttpHandlers();

    //连接登录回包信号
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_login_mod_finish, this,&LoginDialog::slot_http_login_mod_finish);
    //连接tcp连接请求的信号和槽函数
    connect(this, &LoginDialog::sig_connect_tcp,TcpMgr::GetInstance().get(),&TcpMgr::slot_tcp_connect);
    //连接客户端连接成功的信号
    connect(TcpMgr::GetInstance().get(),&TcpMgr::sig_con_success,this,&LoginDialog::slot_tcp_con_finish);
    //连接TCP管理者发出登录失败的信号
    connect(TcpMgr::GetInstance().get(),&TcpMgr::sig_login_failed, this, &LoginDialog::slot_login_failed);
}

LoginDialog::~LoginDialog()
{
    qDebug() << "destruct LoginDlg";
    delete ui;
}

void LoginDialog::initHeader()
{
    //loading picture
    QPixmap originalPixmap(":/res/head_1.jpg");
    //set auto scaled
    qDebug() << originalPixmap.size() << ui->head_label->size();
    originalPixmap = originalPixmap.scaled(ui->head_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    //创建一个和原始图片相同大小的QPixmap,用于绘制圆角图片
    QPixmap roundedPixmap(originalPixmap.size());
    roundedPixmap.fill(Qt::transparent);//用透明色填充

    QPainter painter(&roundedPixmap);
    //设置抗锯齿和平滑度，使圆角更平滑
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    //使用QPainterPath设置圆角
    QPainterPath path;
    path.addRoundedRect(0, 0, originalPixmap.width(), originalPixmap.height(), 10, 10);
    painter.setClipPath(path);

    //将原始图片绘制到roundedPixmap上
    painter.drawPixmap(0, 0, originalPixmap);
    //设置绘制好的圆角图片到QLabel上
    ui->head_label->setPixmap(roundedPixmap);
}

bool LoginDialog::checkEmailValid()
{
    auto email = ui->email_edit->text();
    if(email.isEmpty()){
        qDebug() << "email empty";
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("email can't ben empty"));
        return false;
    }
    DelTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool LoginDialog::checkPwdValid()
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

bool LoginDialog::enableBtn(bool enabled){
    ui->login_btn->setEnabled(enabled);
    ui->register_btn->setEnabled(enabled);
    return true;
}

void LoginDialog::showTip(QString str, bool b_ok)
{
    if(b_ok){
        ui->err_tip->setProperty("state", "normal");
    }else{
        ui->err_tip->setProperty("state", "err");
    }
    ui->err_tip->setText(str);
    repolish(ui->err_tip);
}

void LoginDialog::AddTipErr(TipErr te, QString tips)
{
    tip_err_[te] = tips;
    showTip(tips,false);
}

void LoginDialog::DelTipErr(TipErr te)
{
    tip_err_.remove(te);
    if(tip_err_.empty()){
        ui->err_tip->clear();
        return;
    }
    showTip(tip_err_.first(),false);
}

void LoginDialog::initHttpHandlers()
{
    _handlers.insert(ReqId::ID_LOGIN_USER,[this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"),false);
            enableBtn(true);
            return;
        }
        auto email = jsonObj["email"].toString();
        //发送信号通知tcpMgr发送长链接
        ServerInfo si;
        si.Uid = jsonObj["uid"].toInt();
        si.Host = jsonObj["host"].toString();
        si.Port = QJsonValue(jsonObj["port"]).toVariant().toString();
        si.Token = jsonObj["token"].toString();

        _uid = si.Uid;
        _token = si.Token;
        qDebug()<<"email is " << email <<"uid is" << _uid << "host is"
                 <<si.Host <<"port is" <<si.Port << "token is" << _token;
        //发送信号给tcpMgr 让tcpMgr连接chatServer
        //如果连接成功，tcpMgr会给登录界面发送sig_con_success信号
        //登录执行tcp_finish槽函数
        emit sig_connect_tcp(si);

    });
}

void LoginDialog::slot_forget_pwd()
{
    qDebug()<<"slot forget pwd";
    emit switchReset();
}

void LoginDialog::on_login_btn_clicked()
{
    qDebug() <<"login btn clicked";
    if(checkEmailValid() == false){
        return;
    }
    if(checkPwdValid() == false){
        return;
    }
    //点击登录按钮后禁止再次点击 之后在打开
    enableBtn(false);
    //http
    auto email = ui->email_edit->text();
    auto pwd = ui->pass_edit->text();
    QJsonObject json_obj;
    json_obj["email"] = email;
    json_obj["passwd"] = xorString(pwd);
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/user_login"),json_obj,ReqId::ID_LOGIN_USER,Modules::LOGINMODE);
}

void LoginDialog::slot_http_login_mod_finish(ReqId id, QString res, ErrorCodes err)
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

//TCPMGR发过来的，tcp连接呈成功的槽函数
void LoginDialog::slot_tcp_con_finish(bool bsuccess)
{
    if(bsuccess){
        showTip(tr("聊天服务登录成功，正在登录"),true);
        QJsonObject jsonObj;
        jsonObj["uid"] = _uid;
        jsonObj["token"] = _token;

        QJsonDocument doc(jsonObj);
        QString jsonString = doc.toJson(QJsonDocument::Indented);

        //通过tcpmgr发送数据 tcpmgr将json数据写入socket reqid:1005
        emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_CHAT_LOGIN, jsonString);
    }else{
        showTip(tr("网络错误"),true);
        enableBtn(true);
    }
}

void LoginDialog::slot_login_failed(int err)
{
    QString result = QString("登录失败，err is &1").arg(err);
    showTip(result,false);
    enableBtn(true);
}



