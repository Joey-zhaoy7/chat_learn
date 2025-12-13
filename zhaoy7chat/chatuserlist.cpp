#include "chatuserlist.h"
#include <QEvent>
#include <QWheelEvent>
#include <QScrollBar>

ChatUserList::ChatUserList(QWidget *parent):
    QListWidget(parent)
{
    Q_UNUSED(parent);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //安装时间过滤器
    this->viewport()->installEventFilter(this);
}

bool ChatUserList::eventFilter(QObject *watched, QEvent *event)
{
    //检查时间是否是鼠标悬浮进入或离开
    if(watched == this->viewport()){
        if(event->type() == QEvent::Enter){
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }else if(event->type() == QEvent::Leave){
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }
    //检查事件是否是鼠标滚轮事件
    if(watched == this->viewport() && event->type() == QEvent::Wheel){
        QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
        int numDegress = wheelEvent->angleDelta().y() / 8;
        int numSteps = numDegress /15 ;
        //设置滚动幅度
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);
        //检查是否滚动到底
        QScrollBar* scrollBar = this->verticalScrollBar();
        int maxScrollValue = scrollBar->maximum();
        int currentValue = scrollBar->value();

        if(maxScrollValue - currentValue <= 0){
            qDebug() << "load more chat user";
            emit sig_loading_chat_user();
        }
        return true;
    }
    return QListWidget::eventFilter(watched, event);

}
