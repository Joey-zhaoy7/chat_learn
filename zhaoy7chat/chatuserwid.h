#ifndef CHATUSERWID_H
#define CHATUSERWID_H
#include "listitembase.h"
#include <QWidget>
#include "userdata.h"

namespace Ui {
class ChatUserWid;
}

class ChatUserWid : public ListItemBase
{
    Q_OBJECT

public:
    explicit ChatUserWid(QWidget *parent = nullptr);
    ~ChatUserWid();

    QSize sizeHint()const override{
        return QSize(250,70);
    }
    // void SetInfo(QString name, QString head, QString msg);
    void SetInfo(std::shared_ptr<UserInfo> user_info);
    void SetInfo(std::shared_ptr<FriendInfo> friend_info);
    void ShowRedPoint(bool bshow);
    // void SetUserInfo(std::shared_ptr<>)
    std::shared_ptr<UserInfo> GetUserInfo();
    //更新消息
    void updateLastMsg(std::vector<std::shared_ptr<TextChatData>> msgs);
private:
    Ui::ChatUserWid *ui;
    // QString _name;
    // QString _head;
    // QString _msg;
    std::shared_ptr<UserInfo> _user_info;
};

#endif // CHATUSERWID_H
