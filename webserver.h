#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "header.h"
#include "utils/utils.h"
#include "timer/lst_timer.h"
#include "threadpool/threadpool.h"
#include "http/http_conn.h"
#include "lock/locker.h"
#include "CGImysql/sql_connection_pool.h"
#include "log/log.h"

const int MAX_EVENT_NUMBER = 10000; //最大事件数
const int MAX_FD = 65536; //最大文件描述符
const int TIMESLOT = 5; //最小超时单位

class WebServer
{
public:
    WebServer();
    ~WebServer();

    //监听客户端连接，epoll模式
    void eventListen();

    //处理监听到的事件
    void eventLoop();

    //封装判断错误
    void judge_error(bool judge,const char *error_str);
    
    //初始化
    void init(int port, string user, string passWord, string databaseName,int log_write,int sql_num, int thread_num);
    //处理请求连接的客户端数据
    void dealclientdata();

    //处理客户端发送过来的数据
    void dealwithread(int sockfd);

    //发送数据
    void dealwithwrite(int sockfd);

    //设置定时器
    void timer(int connfd, struct sockaddr_in client_addr);

    //处理关闭客户端的定时器
    void deal_timer(client_timer *timer, int sockfd);

    //处理信号
    bool dealwithsignal(bool& timeout, bool& stop_server);
    
    //延迟定时器
    void adjust_timer(client_timer *timer);

    //线程池
    void thread_pool();

    //数据库
    void sql_pool();

    //日志
    void log_write();
public:
    //基础
    int m_port;
    int m_epollfd;
    char *m_root;
    int m_log_write;
    int m_pipefd[2];//用来传递信号
    http_conn *users;   //客户端http类

    //epoll_event相关
    epoll_event events[MAX_EVENT_NUMBER];
    int m_listenfd; // 监听套接字文件描述符

    //工具类
    Utils utils;

    //定时器相关
    client_data *users_timer;

    //线程池相关
    threadpool<http_conn> *m_pool;
    int m_thread_num;

    //数据库相关
    connection_pool *m_connPool;
    string m_user;          //登陆数据库用户名
    string m_passWord;      //登陆数据库密码
    string m_databaseName;  //使用的数据库名
    int m_sql_num;

};


#endif