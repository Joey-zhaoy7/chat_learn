#include "MysqlMgr.h"

MysqlMgr::MysqlMgr()
{
}
MysqlMgr::~MysqlMgr()
{
}
int MysqlMgr::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
	return mysqlDao_.RegUser(name, email, pwd);
}

bool MysqlMgr::CheckEmail(const std::string& name, const std::string& email)
{
	return mysqlDao_.CheckEmail(name, email);
}

bool MysqlMgr::UpdatePwd(const std::string& name, const std::string& new_pwd)
{
	return mysqlDao_.UpdatePwd(name, new_pwd);
}

bool MysqlMgr::AddFriendApply(const int& from, const int& to)
{
	return mysqlDao_.AddFriendApply(from, to);
}

bool MysqlMgr::GetApplyList(int touid, std::vector<std::shared_ptr<ApplyInfo>>& applyList, int begin, int limit) {
	return mysqlDao_.GetApplyList(touid, applyList, begin, limit);
}

bool MysqlMgr::GetFriendList(int self_id, std::vector < std::shared_ptr<UserInfo>>& user_info) {
	return mysqlDao_.GetFriendList(self_id, user_info);
}

bool MysqlMgr::AuthFriendApply(const int& from, const int& to)
{
	return mysqlDao_.AuthFriendApply(from, to);
}

bool MysqlMgr::AddFriend(const int& from, const int& to, std::string back_name)
{
	return mysqlDao_.AddFriend(from, to, back_name);
}

std::shared_ptr<UserInfo> MysqlMgr::GetUser(int uid)
{
	return mysqlDao_.GetUser(uid);
}

std::shared_ptr<UserInfo> MysqlMgr::GetUser(std::string name)
{
	return mysqlDao_.GetUser(name);
}

bool MysqlMgr::CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userinfo)
{
	return mysqlDao_.CheckPwd(email, pwd, userinfo);
}
