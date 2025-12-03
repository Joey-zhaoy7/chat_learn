const config_module = require('./config');
const Redis = require('ioredis');

const RedisCli = new Redis({
    host: config_module.redis_host,
    port: config_module.redis_port,
    password: config_module.redis_passwd,
});

RedisCli.on('error', function(err){
    console.log('Redis Error: ' + err);
    RedisCli.quit();
});

async function GetRedis(key){
    try {
        const result = await RedisCli.get(key);
        if(result == null){
            console.log('Redis Key Not Found: ' + key);
            return null;
        }
        console.log('Redis Get Key: ' + key + ' Value: ' + result);
        return result;
    }catch (err) {
        console.log('Redis Get Error: ' + err);
        return null;
    }
}

async function QueryRedis(key){
    try{
        const result = await RedisCli.exists(key);
        if(result === 0){
            console.log('this key is null');
            return null;
        }
        console.log('Redis Query Key: ' + key + ' Exists');
        return result;
    }catch(err){
        console.log('Redis Query Error: ' + err);
        return null;
    }
}

async function SetRedisExpire(key, value, expire){
    try{
        await RedisCli.set(key, value);
        await RedisCli.expire(key, expire);
        return true
    }catch(err){
        console.log('Redis Set expire Error: ' + err);
        return false;
    }
}

function Quit(){
    RedisCli.quit();
}

module.exports = {GetRedis, QueryRedis, SetRedisExpire, Quit}