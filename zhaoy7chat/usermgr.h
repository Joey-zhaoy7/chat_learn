#ifndef USERMGR_H
#define USERMGR_H
#include <QObject>
#include "singleton.h"
#include "userdata.h"
#include <QJsonArray>
#include "global.h"
#include <QMap>
class UserMgr:public QObject, public Singleton<UserMgr>,public std::enable_shared_from_this<UserMgr>
{
    Q_OBJECT
public:
    friend class Singleton<UserMgr>;
    ~UserMgr();
    // void SetName(QString name);
    // void SetUid(int uid);
    void SetToken(QString token);
    QString GetName();
    int GetUid();
    std::vector<std::shared_ptr<ApplyInfo>> GetApplyList();
    bool AlreadyApply(int uid);
    void AddApplyList(std::shared_ptr<ApplyInfo> app);
    void SetUserInfo(std::shared_ptr<UserInfo> user_info);
    void AppendApplyList(QJsonArray array);
    void AppendFriendList(QJsonArray array);
    bool CheckFriendById(int uid);
    //对方同意我的添加请求
    void AddFriend(std::shared_ptr<AuthRsp> auth_rsp);
    //我同意对方的添加请求
    void AddFriend(std::shared_ptr<AuthInfo> auth_info);
    std::shared_ptr<FriendInfo> GetFriendById(int uid);

    std::vector<std::shared_ptr<FriendInfo>> GetChatListPerPage();
    bool IsLoadChatFin();
    void UpdateChatLoadedCount();
    std::vector<std::shared_ptr<FriendInfo>> GetConListPerPage();
    void UpdateContactLoadedCount();
    bool IsLoadConFin();

    std::shared_ptr<UserInfo> GetUserInfo();
    void AppendFriendChatMsg(int friend_id, std::vector<std::shared_ptr<TextChatData>>);

private:
    UserMgr();
    // QString _name;
    QString _token;
    // int _uid;

    std::shared_ptr<UserInfo> _user_info;
    //好友申请列表
    std::vector<std::shared_ptr<ApplyInfo>> _apply_list;
    std::vector<std::shared_ptr<FriendInfo>> _friend_list;
    QMap<int, std::shared_ptr<FriendInfo>> _friend_map;




    //用于列表加载判断
    int _chat_loaded;
    int _contact_loaded;
};

#endif // USERMGR_H
