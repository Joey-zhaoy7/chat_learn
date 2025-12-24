#ifndef CONTACTUSERLIST_H
#define CONTACTUSERLIST_H
#include <QListWidget>
#include "conuseritem.h"
class ConUserItem;

//联系人列表 里面有多种item
class ContactUserList : public QListWidget
{
    Q_OBJECT
public:
    ContactUserList(QWidget* parent = nullptr);
    void ShowRedPoint(bool bshow = true);
protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
private:
    //添加好友
    void addContactUserList();
public slots:
    //处理点击item
    void slot_item_clicked(QListWidgetItem* item);
    // //处理添加好友成功 tcp回传
    // void slot_add_auth_friend(std::shared_ptr<AuthInfo>);
    // //处理同意添加好友 更新列表
    // void slot_auth_rsp(std::shared_ptr<AuthRsp>);
signals:
    //加载联系人
    void sig_loading_contact_user();
    //切换好友界面
    void sig_switch_apply_friend_page();
    //切换申请好友界面
    void sig_switch_friend_info_page();
private:
    //每一个item
    ConUserItem* _add_friend_item;
    //分组
    QListWidgetItem* _groupitem;
};

#endif // CONTACTUSERLIST_H
