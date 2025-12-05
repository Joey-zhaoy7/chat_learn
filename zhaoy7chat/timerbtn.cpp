#include "timerbtn.h"


TimerBtn::TimerBtn(QWidget *parent):
    QPushButton(parent), counter_(10)
{
    //Timerbtn回收时 QTimer也被回收
    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, [this](){
        counter_--;
        if(counter_ <= 0){
            timer_ -> stop();
            counter_ = 10;
            this->setText("获取");
            this->setEnabled(true);
            return;
        }
        this->setText(QString::number(counter_));
    });

}

TimerBtn::~TimerBtn()
{
    timer_->stop();
}

void TimerBtn::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton){
        qDebug() << "MyButton was released!";
        this->setEnabled(false);
        this->setText(QString::number(counter_));
        //每隔1s发送一个timeout信号
        timer_->start(1000);
        emit clicked();
    }
    QPushButton::mouseReleaseEvent(e);
}
