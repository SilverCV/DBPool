//
// Created by lover on 2021/5/15.
//
#include "DBPool.h"

DBPool* DBPool::_pool = nullptr;

DBPool *DBPool::getInstance() {
    if (_pool == nullptr){
        _pool = new DBPool("xxx",3306,"xxx","xxx","test");
    }
    return _pool;
}

std::shared_ptr<MYSQL> DBPool::getConnection() {
    std::unique_lock<std::mutex> lck(_m);
    auto scp = _connList.back();
    _connList.pop_back();
    return scp;
}

void DBPool::retConnection(std::shared_ptr<MYSQL> &conn) {
    std::unique_lock<std::mutex> lck(_m);
    _connList.push_back(std::move(conn));
}

DBPool::~DBPool() {
    DestroyPool();
}

DBPool::DBPool(const std::string &ip, unsigned int port, const std::string &name, const std::string &passwd,
               const std::string &db) :_ip(ip),_port(port),_username(name),_password(passwd),_dbName(db),_poolSize(POOLSIZE)
               {
        //init
        initDBPool(_poolSize/2);
}

void DBPool::initDBPool(int size) {
    std::unique_lock<std::mutex> lck(_m);
    CreateConnections(size);
}

void DBPool::DestroyPool() {
    for (auto&& con : _connList) {
        mysql_close(con.get());
    }
}

void DBPool::freeOneConnection() {
    std::unique_lock<std::mutex> lck(_m);
    if (_connList.empty()) {return;}
    auto con = std::move(_connList.back());
    _connList.pop_back();
    mysql_close(con.get());
    --_poolSize;
}

void DBPool::expandPool(int size) {
    std::unique_lock<std::mutex> lck(_m);
    _poolSize += size;
}

void DBPool::reducePool(int size) {
    std::unique_lock<std::mutex> lck(_m);
    if (size > _poolSize){
        DestroyPool();
        _poolSize = 0;
    }else{
        for (int i = 0; i < size; ++i) {
            freeOneConnection();
        }
    }
}

void DBPool::CreateConnections(int n) {
    for (int i = 0; i < n; ++i) {
        MYSQL conn;
        if (mysql_init(&conn) == NULL ){
            printf("fail to init connection\n");
            exit(-1);
        }
        MYSQL* con = mysql_real_connect(&conn,_ip.c_str(),_username.c_str(),_password.c_str(),
                                        _dbName.c_str(),_port,NULL,0);
        if (con == NULL){
            printf("fail to create connection\n");
            exit(-1);
        }
        std::shared_ptr<MYSQL> csp(con,[](MYSQL* con){
            mysql_close(con);
        });
        _connList.push_back(std::move(csp));
    }
}

int DBPool::getPoolSize() const {
    return _connList.size();
}


