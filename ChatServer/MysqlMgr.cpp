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

std::shared_ptr<UserInfo> MysqlMgr::GetUser(int uid)
{
	return mysqlDao_.GetUser(uid);
}

bool MysqlMgr::CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userinfo)
{
	return mysqlDao_.CheckPwd(email, pwd, userinfo);
}
