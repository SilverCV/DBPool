//
// Created by lover on 2021/5/15.
//

#ifndef CDBPOOL_DBPOOL_H
#define CDBPOOL_DBPOOL_H
#include <mysql/mysql.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#define BUFFLEN 64
#define MAXSIZE 20
typedef struct Connection{
    MYSQL fd;
    MYSQL  *_mysqlfd;
    enum{
        CONNECTED,DISCONNECTED
    }sql_state;

    enum {
        USED,UNUSED
    }state;

    enum {INPOOL,NOTINPOOL}attr;
}Connection_t;

typedef struct DBPool{
    int _maxSize;
    int _curSize;
    Connection_t _pool[MAXSIZE];


    char _ip[16];
    unsigned int _port;
    char _userName[BUFFLEN];
    char _password[BUFFLEN];
    char _dbName[BUFFLEN];

    pthread_mutex_t _m;
    int _closed;

}DBPool_t;

//init DBPool
DBPool_t *initDBPool(const char* ip,unsigned int port,const char* userName,const char* password,const char *dbName,int size){
    DBPool_t * pool = (DBPool_t*)malloc(sizeof(DBPool_t));
    if(!pool){
        printf("create pool failed\n");
        exit(-1);
    }
    if (size > MAXSIZE){
        size = MAXSIZE;
    }
    pool->_curSize = 0;
    pool->_maxSize = 0;
    pool->_port = port;
    pool->_closed = 0;
    strcpy(pool->_ip,ip);
    strcpy(pool->_userName,userName);
    strcpy(pool->_password,password);
    strcpy(pool->_dbName,dbName);
    //
    pthread_mutex_init(&pool->_m,NULL);

    pthread_mutex_lock(&pool->_m);

    for (int i = 0; i < size; ++i) {
       Connection_t conn;
       conn._mysqlfd = mysql_init(&conn.fd);
       if (conn._mysqlfd == NULL) {
           pthread_mutex_unlock(&pool->_m);
           printf("fail to init mysql connection\n");
           exit(-1);
       }
       if (mysql_real_connect(conn._mysqlfd,ip,userName,password,dbName,port,NULL,0) == NULL){
           pthread_mutex_unlock(&pool->_m);
           printf("fail to connect mysql\n");
           return -1;
       }
       conn.sql_state = CONNECTED;
       conn.state = UNUSED;
       conn.attr = INPOOL;
       pool->_pool[i] = conn;
       pool->_curSize++;
    }
    pthread_mutex_unlock(&pool->_m);
    return pool;
}
//destroyPool
void DestroyPool(DBPool_t* pool){
    pthread_mutex_lock(&pool->_m);
    for (int i = 0; i < pool->_curSize; ++i) {
       pool->_pool[i]._mysqlfd = NULL;
       mysql_close(&pool->_pool[i].fd);
    }
    pthread_mutex_unlock(&pool->_m);
    //release
    free(pool);
    pool = NULL;
}
//
Connection_t getConnection(DBPool_t *pool){
    pthread_mutex_lock(&pool->_m);
    for (int i = pool->_curSize-1; i >= 0; --i) {
        if (pool->_pool[i].state == UNUSED){
            pool->_pool[i].state = USED;
            pool->_curSize--;
            pthread_mutex_unlock(&pool->_m);
            return pool->_pool[i];
        }
    }
    Connection_t conn;
    conn._mysqlfd = mysql_init(&conn.fd);
    if (conn._mysqlfd == NULL) {
        pthread_mutex_unlock(&pool->_m);
        printf("fail to init mysql connection\n");
        exit(-1);
    }
    if (mysql_real_connect(conn._mysqlfd,pool->_ip,pool->_userName,pool->_password,pool->_dbName,pool->_port,NULL,0) == NULL){
        pthread_mutex_unlock(&pool->_m);
        printf("fail to connect mysql\n");
        exit(-1);
    }
    conn.attr = NOTINPOOL;
    pthread_mutex_unlock(&pool->_m);
    return conn;

}
//
void RetConnection(DBPool_t* pool,Connection_t *conn){
    pthread_mutex_lock(&pool->_m);
    if(conn == NULL){
        pthread_mutex_unlock(&pool->_m);
        return;
    }
    if (conn->attr == NOTINPOOL){
        mysql_close(conn->_mysqlfd);
        free(conn);
        conn = NULL;
        pthread_mutex_unlock(&pool->_m);
    }
    pool->_pool[pool->_curSize++].state = UNUSED;
    //free(conn);
    conn = NULL;
    pthread_mutex_unlock(&pool->_m);
}

#endif //CDBPOOL_DBPOOL_H
