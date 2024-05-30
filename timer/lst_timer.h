#ifndef LST_TIMER
#define LST_TIMER

#include "../header.h"

class client_timer;

struct client_data
{
    sockaddr_in address;
    int sockfd;
    client_timer *timer;
};

class client_timer
{
public:
    client_timer() : prev(NULL),next(NULL){}

    //关闭套接字
    void close_sockfd(int m_eppollfd, client_data *user_data);
public:
    //超时的时间
    time_t expire;
    //回调函数，关闭超时的连接
    client_data *user_data;
    client_timer *prev;
    client_timer *next;
};

class sort_timer_lst
{
public:
    sort_timer_lst();
    ~sort_timer_lst();
    void add_timer(client_timer *timer);

    //删除定时器
    void del_timer(client_timer *timer);

    //查看客户端是否超时
    void tick(int m_eppollfd);

    //调整定时器链表
    void adjust_timer(client_timer *timer);

private:
    void add_timer(client_timer *timer, client_timer *head);
    //记录链表的头
    client_timer *head;
    //记录链表的尾
    client_timer *tail;
};

#endif