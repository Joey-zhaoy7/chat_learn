#ifndef SEARCHLIST_H
#define SEARCHLIST_H
#include <QListWidget>
#include <QEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include "loadingdlg.h"
#include "userdata.h"
#include "global.h"
class SearchList:public QListWidget
{
    Q_OBJECT
public:
    SearchList(QWidget* parent = nullptr);
    void CloseFindDlg();
    void SetSearchEdit(QWidget* edit);
protected:
    //鼠标进入
    bool eventFilter(QObject* watched, QEvent* event)override;
private:
    void waitPending(bool pending = true);//阻塞
    bool _send_pending;//添加好友时设为true，界面不能操作，当收到服务器回包时设为false
    void addTipItem();
    std::shared_ptr<QDialog> _find_dlg;//可以接受QDIaglog所有的子类对象
    QWidget* _search_edit;//编辑框
    LoadingDlg* _loadingDialog;
private slots:
    //点击item之后的显示效果
    void slot_item_clicked(QListWidgetItem* item);
    //接收到user search信号 之后的处理
    void slot_user_search(std::shared_ptr<SearchInfo> si);
signals:
    void sig_jump_chat_item(std::shared_ptr<SearchInfo> si);

};

#endif // SEARCHLIST_H
