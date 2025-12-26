#ifndef CHATDIALOG_H
#define CHATDIALOG_H
#include "global.h"
#include <QDialog>
#include "statewidget.h"
#include "userdata.h"
#include <QListWidgetItem>
namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();
    void addChatUserList();
    void ClearLabelState(StateWidget* lb);
protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void handleGlobalMousePress(QMouseEvent* event);
private:
    void ShowSearch(bool bsearch);
    void AddLBGroup(StateWidget* lb);
    void SetSelectChatItem(int uid = 0);
    void SetSelectChatPage(int uid = 0);
    void loadMoreChatUser();
    void loadMoreConUser();
    void UpdateChatMsg(std::vector<std::shared_ptr<TextChatData>> msgdata);
    Ui::ChatDialog *ui;
    ChatUIMode _mode;
    ChatUIMode _state;
    bool _b_loading;
    QList<StateWidget*> _lb_list;
    QMap<int, QListWidgetItem*> _chat_items_added;
    int _cur_chat_uid;//当前的选中项
    QWidget* _last_widget;
private slots:
    void slot_loading_chat_user();
    void slot_loading_contact_user();
public slots:
    void slot_side_chat(); //释放side_chat进行的操作
    void slot_side_contact();//释放side_contact进行的操作
    void slot_text_changed(const QString& str);//搜索输入框变化
public slots:
    //点击添加好友 收到回调后 自己客户端进行的处理
    void slot_apply_friend(std::shared_ptr<AddFriendApply>);//申请添加好友
    // //对方同意我的添加好友请求，处理添加好友成功
    void slot_add_auth_friend(std::shared_ptr<AuthInfo>);
    //处理自己同意对方的添加好友请求 更新列表
    void slot_auth_rsp(std::shared_ptr<AuthRsp>);
    //搜索到已经添加的好友，跳转到聊天界面
    void slot_jump_chat_item(std::shared_ptr<SearchInfo> si);
    //点击联系人item
    void slot_friend_info_page(std::shared_ptr<UserInfo> user_info);
    //切换到申请添加好友界面
    void slot_switch_apply_friend_page();
    //点击联系人item 跳转到聊天界面
    void slot_jump_chat_item_from_infopage(std::shared_ptr<UserInfo> user_info);
    //点击聊天列表item 切换右侧页面
    void slot_item_clicked(QListWidgetItem* item);
    //
    void slot_append_send_chat_msg(std::shared_ptr<TextChatData> msg);

    void slot_text_chat_msg(std::shared_ptr<TextChatMsg> msg);
};

#endif // CHATDIALOG_H
