#ifndef WEBSERVER_H
#define WEBSERVER_H
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>  
#include <sys/socket.h>
#include <iostream>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
using namespace std;

const int MAX_EVENT_NUMBER = 10000; //最大事件数

class WebServer
{
public:
    //WebServer();
    //~WebServer();
    //监听客户端连接，epoll模式
    void eventListen();
    //封装判断错误
    void judge_error(bool judge,const char *error_str);
    //初始化
    void init(int port);
public:
    //基础
    int m_port;
    int m_epollfd;

    //epoll_event相关
    int m_listenfd; // 监听套接字文件描述符

};


#endif