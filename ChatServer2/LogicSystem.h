#pragma once
#include "Singleton.h"
#include <queue>
#include <thread>
#include <queue>
#include <map>
#include <functional>
#include "const.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <unordered_map>
#include "data.h"
#include "CSession.h"


typedef  function<void(std::shared_ptr<CSession>, const short& msg_id, const string& msg_data)> FunCallBack;
class LogicSystem :public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;
public:
	~LogicSystem();
	void PostMsgToQue(std::shared_ptr <LogicNode> msg);
private:
	LogicSystem();
	void DealMsg();
	void RegisterCallBacks();
	void LoginHandler(std::shared_ptr<CSession>, const short& msg_id, const string& msg_data);
	void AddFriendApply(std::shared_ptr<CSession>, const short& msg_id, const string& msg_data);
	//收到客户端在添加好友界面点击接受？
	void AuthFriendApply(std::shared_ptr<CSession> session, const short& msg_id, const string& msg_data);
	//发送信息请求
	void DealChatTextMsg(std::shared_ptr<CSession> session, const short& msg_id, const string& msg_data);
	void SearchInfo(std::shared_ptr<CSession>, const short& msg_id, const string& msg_data);
	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);
	bool isPureDigit(const std::string& str);
	void GetUserByUid(std::string uid_str, Json::Value& rtvalue);
	void GetUserByName(std::string uid_str, Json::Value& rtvalue);
	bool GetFriendApplyInfo(int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& list);
	bool GetFriendList(int self_id, std::vector <std::shared_ptr<UserInfo>>& user_list);
	std::thread _worker_thread;
	std::queue<std::shared_ptr<LogicNode>> _msg_que;
	std::mutex _mutex;
	std::condition_variable _consume;
	bool _b_stop;
	std::map<short, FunCallBack> _fun_callbacks;
	//std::unordered_map<int, std::shared_ptr<UserInfo>> _users;
};

