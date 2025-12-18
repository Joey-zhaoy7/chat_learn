#include "chatpage.h"
#include "ui_chatpage.h"
#include <QStyleOption>
#include <QPainter>
#include <QPixmap>
#include "chatitembase.h"
#include "picturebubble.h"
#include "textbubble.h"

ChatPage::ChatPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatPage)
{
    ui->setupUi(this);
    //设置按钮样式
    ui->receive_btn->SetState("normal","hover","press");
    ui->send_btn->SetState("normal","hover","press");

    //设置图标样式
    ui->emo_lb->SetState("normal","hover","press","normal","hover","press");
    ui->file_lb->SetState("normal","hover","press","normal","hover","press");

}

ChatPage::~ChatPage()
{
    delete ui;
}

void ChatPage::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ChatPage::on_send_btn_clicked()
{
    auto pTexEdit = ui->chatEdit;
    ChatRole role = ChatRole::Self;
    QString userName = QStringLiteral("云衫");
    QString userIcon = ":/res/head_1.jpg";

    const QVector<MsgInfo>& msgList = pTexEdit->getMsgList();
    for(int i =0; i<msgList.size();i++){
        QString type = msgList[i].msgFlag;
        ChatItemBase* pChatitem = new ChatItemBase(role);
        pChatitem->setUserName(userName);
        pChatitem->setUserIcon(QPixmap(userIcon));
        QWidget* pBubble = nullptr;
        if(type == "text"){
            pBubble = new TextBubble(role, msgList[i].content);
        }else if(type =="image"){
            pBubble = new PictureBubble(QPixmap(msgList[i].content), role);
        }else if(type == "file"){

        }
        if(pBubble != nullptr){
            pChatitem->setWidget(pBubble);
            ui->chat_data_list->appendChatItem(pChatitem);
        }

    }
}

