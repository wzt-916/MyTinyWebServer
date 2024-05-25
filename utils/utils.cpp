#include "utils.h"
int *Utils::utils_pipe;
Utils::Utils()
{
    
}
//初始化
void Utils::init(int timeslot)
{
    utils_TIMESLOT = timeslot;
}

void Utils::setnonblocking(int fd)
{
    //将cfd设置为非阻塞
    int flag = fcntl(fd, EPOLLET);
    flag |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flag);
}

//定时处理任务，重新定时以不断触发SIGALRM信号
void Utils::timer_handler(int m_eppollfd)
{
    m_timer_lst.tick(m_eppollfd);
    alarm(utils_TIMESLOT);
}

//信号处理函数
void Utils::sig_handler(int sig)
{
    int msg = sig;
    send(utils_pipe[1], (char *)&msg, 1 , 0);
}

//设置信号函数
void Utils::addsig(int sig, void(handler)(int))
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = handler;
    //屏蔽所有信号
    sigfillset(&sa.sa_mask);

    sigaction(sig, &sa, NULL);
    
}

