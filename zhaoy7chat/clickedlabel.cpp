#include "clickedlabel.h"
#include <QMouseEvent>


ClickedLabel::ClickedLabel(QWidget *parent):
    QLabel(parent), _curstate(ClickLbState::Normal)
{
    this->setCursor(Qt::PointingHandCursor);
}

void ClickedLabel::enterEvent(QEnterEvent *event)
{
    // 在这里处理鼠标悬停进入的逻辑
        //不可见时 当鼠标进入label范围 state->_normal_hover qss显示unvisible-hover样式
    if(_curstate == ClickLbState::Normal){
        qDebug()<<"enter , change to normal hover: "<< _normal_hover;
        setProperty("state",_normal_hover);
        repolish(this);
        update();

    }else{
        //可见时
        qDebug()<<"enter , change to selected hover: "<< _selected_hover;
        setProperty("state",_selected_hover);
        repolish(this);
        update();
    }

    QLabel::enterEvent(event);
}

void ClickedLabel::leaveEvent(QEvent *event)
{
    // 在这里处理鼠标悬停离开的逻辑
    if(_curstate == ClickLbState::Normal){
        qDebug()<<"leave , change to normal : "<< _normal;
        setProperty("state",_normal);
        repolish(this);
        update();

    }else{
        qDebug()<<"leave , change to normal hover: "<< _selected;
        setProperty("state",_selected);
        repolish(this);
        update();
    }
    QLabel::leaveEvent(event);
}

void ClickedLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if(_curstate == ClickLbState::Normal){
            _curstate = ClickLbState::Selected;
            qDebug()<<"press, normal_hover -> selected_press: "<< _selected_press;
            // _curstate = ClickLbState::Selected;
            setProperty("state",_selected_press);
            repolish(this);
            update();

        }else{
            _curstate = ClickLbState::Normal;
            qDebug()<<"press , selected_hover -> normal_press:: "<< _normal_press;
            // _curstate = ClickLbState::Normal;
            setProperty("state",_normal_press);
            repolish(this);
            update();
        }
        // return;
        // emit clicked();
        // return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QLabel::mousePressEvent(event);
}

// void ClickedLabel::mouseReleaseEvent(QMouseEvent *event)
// {
//     if (event->button() == Qt::LeftButton) {
//         if(_curstate == ClickLbState::Normal){
//             qDebug()<<"release , change to normal hover: "<< _selected_hover;
//             _curstate = ClickLbState::Selected;
//             setProperty("state",_normal_hover);
//             repolish(this);
//             update();

//         }else{
//             qDebug()<<"release , change to selected hover: "<< _normal_hover;
//             _curstate = ClickLbState::Normal;
//             setProperty("state",_selected_hover);
//             repolish(this);
//             update();
//         }
//         // return;
//         emit clicked();
//         return;
//     }
//     // 调用基类的mouseReleaseEvent以保证正常的事件处理
//     QLabel::mouseReleaseEvent(event);
// }

void ClickedLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 判断鼠标是否在控件内释放
        if (this->rect().contains(event->pos())) {
            // 根据当前状态显示对应的悬停样式
            if(_curstate == ClickLbState::Normal){
                qDebug()<<"release, normal_press -> normal_hover (鼠标仍在控件内)";
                setProperty("state", _normal_hover);
            } else {
                qDebug()<<"release, selected_press -> selected_hover (鼠标仍在控件内)";
                setProperty("state", _selected_hover);
            }

            repolish(this);
            update();

            emit clicked(); // 发送点击信号
        } else {
            // 鼠标在控件外释放，显示普通样式
            if(_curstate == ClickLbState::Normal){
                qDebug()<<"release, normal_press -> normal (鼠标在控件外)";
                setProperty("state", _normal);
            } else {
                qDebug()<<"release, selected_press -> selected (鼠标在控件外)";
                setProperty("state", _selected);
            }

            repolish(this);
            update();

            // 如果需要在控件外释放也发送信号，可以取消下面注释
            // emit clicked();
        }
        return;
    }
    QLabel::mouseReleaseEvent(event);
}

void ClickedLabel::SetState(QString normal, QString hover, QString press, QString select, QString select_hover, QString select_press)
{
    _normal = normal;
    _normal_hover = hover;
    _normal_press = press;

    _selected = select;
    _selected_hover = select_hover;
    _selected_press = select_press;

    setProperty("state",normal);
    repolish(this);
}

ClickLbState ClickedLabel::GetCurState(){
    return _curstate;
}
