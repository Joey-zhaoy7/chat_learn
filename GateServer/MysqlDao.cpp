#include "MysqlDao.h"
#include "ConfigMgr.h"
MysqlDao::MysqlDao()
{
	auto& cfg = ConfigMgr::Inst();
	const auto& host = cfg["Mysql"]["Host"];
	const auto& port = cfg["Mysql"]["Port"];
	const auto& pwd = cfg["Mysql"]["Passwd"];
	const auto& schema = cfg["Mysql"]["Schema"];
	const auto& user = cfg["Mysql"]["User"];
	pool_.reset(new MysqlPool(host + ":" + port, user, pwd, schema, 5));
}

MysqlDao::~MysqlDao()
{
	pool_->Close();
}

bool MysqlDao::UpdatePwd(const std::string& name, const std::string& new_pwd)
{
	auto con = pool_->getConnection();
	try {
		if (con == nullptr) return false;

		//SQL语句更新用户密码
		std::unique_ptr<sql::PreparedStatement> pstmt(con->con_->prepareStatement("UPDATE user SET pwd = ? WHERE name = ?"));
		pstmt->setString(1, new_pwd);
		pstmt->setString(2, name);
		int affected_rows = pstmt->executeUpdate();
		std::cout << "UpdatePwd affected rows: " << affected_rows << std::endl;
		pool_->returnConnection(std::move(con));
		return affected_rows > 0;
	}
	catch (sql::SQLException& e) {
		pool_->returnConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

bool MysqlDao::CheckEmail(const std::string& name, const std::string& email)
{
	auto con = pool_->getConnection();
	try {
		if (con == nullptr) return false;

		//SQL语句查询用户名对应的邮箱
		std::unique_ptr<sql::PreparedStatement> pstmt(con->con_->prepareStatement("SELECT email FROM user WHERE name =?"));
		pstmt->setString(1, name);
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		while (res->next()) {
			std::cout << "Email from DB: " << res->getString("email") << std::endl;
			if (email != res->getString("email")){
				pool_->returnConnection(std::move(con));
				return false;
			}
			pool_->returnConnection(std::move(con));
			return true;
		}
		return true;
	}catch (sql::SQLException& e) {
		pool_->returnConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
	return true;
}

int MysqlDao::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
	//auto* con = pool_->getConnection();//return unique_ptr, auto*->auto
	auto con = pool_->getConnection();
	try {
		if( con == nullptr ) {
			//pool_->returnConnection(std::move(con));
			return false;
		}
		//std::unique_ptr<sql::PreparedStatement> stmt2(con->con_-)

		std::unique_ptr<sql::PreparedStatement> stmt(con->con_->prepareStatement("CALL reg_user(?,?,?,@result)"));
		stmt->setString(1, name);
		stmt->setString(2, email);
		stmt->setString(3, pwd);
		stmt->execute();

		//Gemini【修正】消费掉存储过程调用可能产生的任何结果集
        // 确保连接在执行下一个查询前是干净的
		
		//while (stmt->getMoreResults()) {
		//	// getResultSet() 即使是空结果集，也会将其从连接上取走
		//	std::unique_ptr<sql::ResultSet> rs(stmt->getResultSet());
		//}

		std::unique_ptr<sql::Statement> stmtResult(con->con_->createStatement());
		std::unique_ptr<sql::ResultSet> res(stmtResult->executeQuery("SELECT @result AS result"));

		//res结果集不为空 且有数据	
		if (res->next()) {
			int result = res->getInt("result");
			std::cout << "RegUser result: " << result << std::endl;
			pool_->returnConnection(std::move(con));
			return result;
		}

		pool_->returnConnection(std::move(con));
		return -1;
	}
	catch(sql::SQLException &e){
		pool_->returnConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return -1;
	}
}

MysqlPool::MysqlPool(const std::string& url, const std::string& user, const std::string& passwd, const std::string& schema, int poolSize):
	url_(url), user_(user), passwd_(passwd), schema_(schema), poolSize_(poolSize), b_stop_(false)
{
	try {
		for (size_t i = 0; i < poolSize_; i++) {
			sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
			
			//编译失败，unique_ptr 不支持拷贝构造
			//std::unique_ptr<sql::Connection> con = std::unique_ptr<sql::Connection>(driver->connect(url_, user_, passwd_));
			
			//正确写法 直接在构造函数中传入参数 或者使用 std::make_unique (C++14 之后支持)
			/*std::unique_ptr<sql::Connection> conn(
				//driver->connect(url_, user_, passwd_)
			//);*/

			auto* conn = driver->connect(url_, user_, passwd_);

			// 设置默认数据库
			conn->setSchema(schema_);

			//获取当前时间戳
			auto currentTime = std::chrono::system_clock::now().time_since_epoch();
			long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
			// 这里可以将 conn 存入连接池容器
			//queue_pool_.push(std::move(conn));

			//conn放入参数中，作为原始指针，SqlConnection构造函数中再转为unique_ptr,以保证conn被正确释放
			queue_pool_.push(std::make_unique<SqlConnection>(conn, timestamp));
		}
		check_thread_ = std::thread([this]() {
			while (!b_stop_) {
				checkConnection();
				std::this_thread::sleep_for(std::chrono::seconds(180));
			}
			});
		check_thread_.detach();

	}
	catch (sql::SQLException& e) {
		std::cout << "mysql pool init failed" << std::endl;
	}

}


std::unique_ptr<SqlConnection> MysqlPool::getConnection() {
	//lock为什么用unique_lock？
	// 因为condition_variable的wait需要传入unique_lock类型的锁
	std::unique_lock<std::mutex> lock(mutex_);

	cond_.wait(lock, [this] {
		if (b_stop_) return true;
		return !queue_pool_.empty();
		});
	if (b_stop_) { return nullptr; }

	//auto con = std::move(queue_pool_.front());
	std::unique_ptr<SqlConnection> con(std::move(queue_pool_.front()));
	queue_pool_.pop();
	return con;
}

void MysqlPool::returnConnection(std::unique_ptr<SqlConnection> con) {
	//lock_guard为什么不能用unique_lock？
	// 因为这里不需要condition_variable的wait功能，只需要简单的加锁解锁
	std::lock_guard<std::mutex> lock(mutex_);
	if (b_stop_) { return; }
	queue_pool_.push(std::move(con));
	cond_.notify_one();
}

void MysqlPool::Close() {
	std::lock_guard<std::mutex> lock(mutex_);
	b_stop_ = true;
	cond_.notify_all();
}
MysqlPool::~MysqlPool() {
	std::lock_guard<std::mutex> lock(mutex_);
	while(!queue_pool_.empty()) {
		queue_pool_.pop();
	}
}

void MysqlPool::checkConnection()
{
	std::lock_guard<std::mutex> guard(mutex_);
	int pool_size = queue_pool_.size();
	auto currentTime = std::chrono::system_clock::now().time_since_epoch();
	long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
	for (int i = 0; i < pool_size; i++) {
		//auto con = std::move(queue_pool_.front());
		std::unique_ptr<SqlConnection> con(std::move(queue_pool_.front()));
		queue_pool_.pop();
		Defer defer([this, &con]() {
			queue_pool_.push(std::move(con));
			});
		if (timestamp - con->last_operate_time_ < 10) {
			continue;
		}
		try {

			// 使用 executeQuery 并显式消费结果集，确保连接没有残留 resultset
			std::unique_ptr<sql::Statement> stmt(con->con_->createStatement());
			std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery("SELECT 1"));
			// 消费结果集，防止残留
			if (rs && rs->next()) {
				// no-op
			}
			con->last_operate_time_ = timestamp;
			std::cout << "execue timer alive query, cur is " << timestamp << ", conn=" << con->con_.get() << std::endl;

			/*std::unique_ptr<sql::Statement> stmt(con->con_->createStatement());
			stmt->execute("SELECT 1");
			con->last_operate_time_ = timestamp;
			std::cout << "execue timer alive query,cur is"<<timestamp << std::endl;*/
		}
		catch (sql::SQLException& e) {
			std::cout << "Error keeping connection alive (conn=" << con->con_.get() << "): " << e.what() << std::endl;

			// 出现问题：重建底层连接以恢复干净状态
			try {
				sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
				auto* newconn = driver->connect(url_, user_, passwd_);
				newconn->setSchema(schema_);
				con->con_.reset(newconn);
				con->last_operate_time_ = timestamp;
				std::cout << "Recreated connection for conn slot, new conn=" << con->con_.get() << std::endl;
			}
			catch (sql::SQLException& e2) {
				std::cout << "Failed to recreate connection: " << e2.what() << std::endl;
			}



			/*std::cout << "Error keeping connection alive: " << e.what() << std::endl;

			sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
			auto* newconn = driver->connect(url_, user_, passwd_);
			newconn->setSchema(schema_);
			con->con_.reset(newconn);
			con->last_operate_time_ = timestamp;*/
		}
	}
}

SqlConnection::SqlConnection(sql::Connection* conn, int64_t lasttime):
	con_(conn), last_operate_time_(lasttime)
{
}
