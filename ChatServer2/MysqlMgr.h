#pragma once
#include "Singleton.h"
#include "MysqlDao.h"
class MysqlMgr : public Singleton<MysqlMgr>
{
	friend class Singleton<MysqlMgr>;
public:
	~MysqlMgr();
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
	bool CheckEmail(const std::string& name, const std::string& email);
	bool CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userinfo);
	bool UpdatePwd(const std::string& name, const std::string& new_pwd);
	bool AddFriendApply(const int& from, const int& to);
	bool GetApplyList(int touid, std::vector<std::shared_ptr<ApplyInfo>>& applyList, int begin, int limit = 10);
	bool GetFriendList(int self_id, std::vector < std::shared_ptr<UserInfo>>& user_info);
	bool AuthFriendApply(const int& from, const int& to);
	bool AddFriend(const int& from, const int& to, std::string back_name);
	std::shared_ptr<UserInfo> GetUser(int uid);
	std::shared_ptr<UserInfo> GetUser(std::string name);
private:
	MysqlMgr();
	MysqlDao mysqlDao_;
};

