#ifndef SQL_CONNECTION_H
#define SQL_CONNECTION_H
#include "../header.h"
#include "../lock/locker.h"

class connection_pool
{
public:
    connection_pool();
    ~connection_pool();
    MYSQL *GetConnection();       //获得数据库连接
    //单例模式
    static connection_pool *GetInstance();
    void init(string url, string User, string PassWord, string DataBaseName,
            int Port, int MaxConn, int close_log);
    //释放当前使用的连接
    bool ReleaseConnection(MYSQL *con);
    //销毁数据库连接池
    void DestroyPool();

private:

    int m_MaxConn;    //最大连接数
    int m_CurConn;    //当前已使用的连接数
    int m_FreeConn;   //当前空闲的连接数
    locker lock;
    list<MYSQL *> connList;  //连接池
    sem reserve;

public:
    string m_url;      //主机地址
    string m_Port;     //数据库端口号
    string m_User;     //登陆数据库用户名
    string m_PassWord; //登陆数据库密码
    string m_DatabaseName; //使用数据库名
};

class connectionRAII
{
public:
    connectionRAII(MYSQL **con, connection_pool *connPool);
    ~connectionRAII();

private:
    MYSQL *conRAII;
    connection_pool *poolRAII;
};

#endif