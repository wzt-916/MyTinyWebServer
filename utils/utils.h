#ifndef UTILS_H
#define UTILS_H

#include "../header.h"
#include "../timer/lst_timer.h"

class Utils
{
public:
    Utils();

    //初始化
    void init(int timeslot);
    //设置非阻塞
    void setnonblocking(int fd);
    
    //设置信号函数
    void addsig(int sig, void(handler)(int));

    //信号处理函数
    static void sig_handler(int sig);

    //定时处理任务，重新定时以不断触发SIGALRM信号
    void timer_handler(int m_eppollfd);

public:
    //定时器相关
    sort_timer_lst m_timer_lst;

    static int *utils_pipe;
    int utils_TIMESLOT;

};

#endif