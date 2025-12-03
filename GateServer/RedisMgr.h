#pragma once
#include "const.h"

class RedisConPool {
public:
    RedisConPool(size_t poolSize, const char* host, int port, const char* pwd);
    ~RedisConPool();
    redisContext* getConnection();
    void returnConnection(redisContext* context);
    void close();
private:
    size_t poolSize_;
    const char* host_;
    int port_;
    std::queue<redisContext*> connections_; //当前使用裸指针，容易内存泄漏

    //原子布尔变量，用于标记连接池是否已经停止。当连接池被关闭时，此变量设置为true，防止新的连接被获取，并等待中心线程的退出
    std::atomic<bool> b_stop_;
    //互斥锁，用于保护对连接队列的并发访问，确保多个线程同时获取或归还连接时的线程安全
    std::mutex mutex_;
    //条件变量，用于在连接池为空时阻塞获取连接的线程，直到有可用连接。当有连接被归还时，通过条件变量通知等待的线程。
    std::condition_variable cond_;
};

class RedisMgr : public Singleton<RedisMgr>
{
    friend class Singleton<RedisMgr>;
public:
    ~RedisMgr();
    //bool Connect(const std::string& host, int port);
    bool Get(const std::string& key, std::string& value);
    bool Set(const std::string& key, const std::string& value);
    bool Auth(const std::string& password);
    bool LPush(const std::string& key, const std::string& value);
    bool LPop(const std::string& key, std::string& value);
    bool RPush(const std::string& key, const std::string& value);
    bool RPop(const std::string& key, std::string& value);
    bool HSet(const std::string& key, const std::string& hkey, const std::string& value);
    bool HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen);
    std::string HGet(const std::string& key, const std::string& hkey);
    bool Del(const std::string& key);
    bool ExistsKey(const std::string& key);
    void Close();
private:
    RedisMgr();
    //redisContext* _connect;
    //redisReply* _reply;

    //pool
    std::unique_ptr<RedisConPool> con_pool_;
};


