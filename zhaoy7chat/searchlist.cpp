#include "searchlist.h"
#include "tcpmgr.h"
#include "adduseritem.h"
#include "listitembase.h"
#include "customizeedit.h"
#include "findsuccessdlg.h"
#include "userdata.h"
SearchList::SearchList(QWidget *parent)
    :_find_dlg(nullptr), _search_edit(nullptr), _send_pending(false)
{
    Q_UNUSED(parent);
    //hide scroll bar
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //install eventfilter
    this->viewport()->installEventFilter(this);
    //connect
    connect(this, &QListWidget::itemClicked, this, &SearchList::slot_item_clicked);
    addTipItem();
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_user_search, this, &SearchList::slot_user_search);
}

void SearchList::CloseFindDlg()
{
    if(_find_dlg){
        _find_dlg->hide();
        _find_dlg = nullptr;
    }
}

void SearchList::SetSearchEdit(QWidget *edit)
{

}

bool SearchList::eventFilter(QObject* watched, QEvent* event)
{
    //enter or leave
    if(watched == this->viewport()){
        if(event->type() == QEvent::Enter){
            //mouse enter, show scroll bar
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }else if(event->type() == QEvent::Leave){
            //mouse leave, hide scroll bar
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    //mousewheel
    if(watched == this->viewport() && event->type() == QEvent::Wheel){
        QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
        int numDegress = wheelEvent->angleDelta().y() / 8;
        int numSteps = numDegress / 15;
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);

        return true; //prevent from transporting event to QListWidget, QListWidget will handle event again
    }

    return QListWidget::eventFilter(watched, event);
}

void SearchList::waitPending(bool pending)
{

}

void SearchList::addTipItem()
{
    auto *invalid_item = new QWidget();
    QListWidgetItem *item_tmp = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item_tmp->setSizeHint(QSize(250,10));
    this->addItem(item_tmp);
    invalid_item->setObjectName("invalid_item");
    this->setItemWidget(item_tmp, invalid_item);
    item_tmp->setFlags(item_tmp->flags() & ~Qt::ItemIsSelectable);


    auto *add_user_item = new AddUserItem();
    QListWidgetItem *item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(add_user_item->sizeHint());
    this->addItem(item);
    this->setItemWidget(item, add_user_item);
}


void SearchList::slot_item_clicked(QListWidgetItem *item)
{
    qDebug() <<" run here slot item clicked";
    QWidget* widget = this->itemWidget(item); //获取自定义的widget对象
    if(!widget){
        qDebug() << "slot item clicked widget is nullptr";
        return;
    }

    //转化为listItemBase
    ListItemBase* customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem){
        qDebug() << "slot item clocked widget is nullptr";
        return;
    }

    auto itemType = customItem->GetItemType();
    if(itemType == ListItemType::INVALID_ITEM){
        qDebug()<< "slot invalid item clicked ";
        return;
    }

    if(itemType == ListItemType::ADD_USER_TIP_ITEM){
        //todo ...
        _find_dlg = std::make_shared<FindSuccessDlg>(this);
        auto si = std::make_shared<SearchInfo>(0,"joey","nick","hello world",1,"test");
        std::dynamic_pointer_cast<FindSuccessDlg>(_find_dlg)->setSearchInfo(si);

        qDebug() << "slot ADD_USER_TIP_ITEM ";
        _find_dlg->show();
        return;
    }

    //清除弹出框
    CloseFindDlg();
}

void SearchList::slot_user_search(std::shared_ptr<SearchInfo> si)
{

}
