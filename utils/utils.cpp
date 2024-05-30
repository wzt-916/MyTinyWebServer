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
    // 获取当前文件描述符的状态标志
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl(F_GETFL)");
        return;
    }

    // 将文件描述符设置为非阻塞模式
    flags |= O_NONBLOCK;
    int result = fcntl(fd, F_SETFL, flags);
    if (result == -1) {
        perror("fcntl(F_SETFL)");
    }
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

