#pragma once

#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/cppconn/prepared_statement.h>
#include "const.h"
#include "data.h"

class SqlConnection {
public:
	SqlConnection(sql::Connection* conn, int64_t lasttime);
	std::unique_ptr<sql::Connection> con_;
	int64_t last_operate_time_;
};

class MysqlPool {
public:
	MysqlPool(const std::string& url, const std::string& user, const std::string& passwd, const std::string& schema, int poolSize);
	std::unique_ptr<SqlConnection> getConnection();
	void returnConnection(std::unique_ptr<SqlConnection> con);
	void Close();
	~MysqlPool();
	void checkConnection();
private:
	std::string url_;
	std::string user_;
	std::string passwd_;
	std::string schema_;
	int poolSize_;
	std::queue<std::unique_ptr<SqlConnection>> queue_pool_;

	//pool parameters
	std::mutex mutex_;
	std::condition_variable cond_;
	std::atomic<bool> b_stop_;
	std::thread check_thread_;
};

//struct UserInfo {
//	int uid;
//	std::string name;
//	std::string email;
//	std::string passwd;
//};

class MysqlDao
{
public:
	MysqlDao();
	~MysqlDao();
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
	bool CheckEmail(const std::string& name, const std::string& email);
	bool CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userinfo);
	bool UpdatePwd(const std::string& name, const std::string& new_pwd);
	bool AddFriendApply(const int& from, const int& to);
	bool GetApplyList(int touid, std::vector<std::shared_ptr<ApplyInfo>>& applyList, int begin, int limit);
	bool GetFriendList(int self_id, std::vector < std::shared_ptr<UserInfo>>& user_info);
	bool AuthFriendApply(const int& from, const int& to);
	bool AddFriend(const int& from, const int& to, std::string back_name);
	std::shared_ptr<UserInfo> GetUser(int uid);
	std::shared_ptr<UserInfo> GetUser(std::string name);
private:
	std::unique_ptr<MysqlPool> pool_;
};

