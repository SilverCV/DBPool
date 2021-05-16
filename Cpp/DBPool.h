//
// Created by lover on 2021/5/15.
//

#ifndef DBPOOL_DBPOOL_H
#define DBPOOL_DBPOOL_H
#include <string>
#include <list>
#include <mysql/mysql.h>
#include <memory>
#include <mutex>
class DBPool{
    const static int POOLSIZE = 20;
public:
    //singleton
    static DBPool* getInstance() ;
    //get one connection
    std::shared_ptr<MYSQL> getConnection();
    //return connection
    void retConnection(std::shared_ptr<MYSQL>& conn);
    //release
    ~DBPool();

private:
    //init
    DBPool(std::string const& ip,unsigned int port,std::string const& name,std::string const& passwd,std::string const& db);
    //init the pool
    void initDBPool(int size);
    //destroy pool
    void DestroyPool();
    //free one connection
    void freeOneConnection();
    //expand pool
    void expandPool(int size);
    //reduce pool
    void reducePool(int size);

    //create n connections
    void CreateConnections(int n);

public:
    //get the num of connection
    int getPoolSize() const;
private:
    std::string _ip;
    std::string _username; //the user name
    std::string _password; // the password
    unsigned int _port; // the port
    std::string _dbName;

    int _poolSize;


    std::list<std::shared_ptr<MYSQL>> _connList;


    static DBPool *_pool;
    std::mutex _m;

};

#endif //DBPOOL_DBPOOL_H
