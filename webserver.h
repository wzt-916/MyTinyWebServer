#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "header.h"
#include "utils/utils.h"
#include "timer/lst_timer.h"

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
    void init(int port);

    //处理请求连接的客户端数据
    void dealclientdata();

    //处理客户端发送过来的数据
    void dealwithread(int sockfd);

    //设置定时器
    void timer(int m_epollfd, int connfd, struct sockaddr_in client_addr);

    //处理关闭客户端的定时器
    void deal_timer(client_timer *timer, int sockfd);

    //处理信号
    bool dealwithsignal(bool& timeout, bool& stop_server);
public:
    //基础
    int m_port;
    int m_epollfd;
    int m_pipefd[2];//用来传递信号

    //epoll_event相关
    epoll_event events[MAX_EVENT_NUMBER];
    int m_listenfd; // 监听套接字文件描述符

    //工具类
    Utils utils;

    //定时器相关
    client_data *users_timer;


};


#endif