#include "webserver.h"

WebServer::WebServer()
{
    //http_conn类对象
    users = new http_conn[MAX_FD];

    //root文件夹路径
    char server_path[200];
    getcwd(server_path, 200);
    char root[6] = "/root";
    m_root = (char *)malloc(strlen(server_path) + strlen(root) + 1);
    strcpy(m_root, server_path);
    strcat(m_root, root);

    //定时器
    users_timer = new client_data[MAX_FD];
}
WebServer::~WebServer()
{

}
void WebServer::init(int port)
{
    printf("开启服务器\n");
    m_port = port;
}

void WebServer::judge_error(bool judge,const char *error_str)
{
    if(judge)
    {
        perror(error_str);
        exit(1);
    }
}
//线程池
void WebServer::thread_pool()
{
    m_pool = new threadpool<http_conn>();
}

void WebServer::timer(int connfd, struct sockaddr_in client_addr)
{
    
    //初始化client_data数据
    //创建定时器，设置回调函数和超时时间，绑定用户数据，将定时器添加到链表中
    users_timer[connfd].address = client_addr;
    users_timer[connfd].sockfd = connfd;

    //存储客户端信息
    client_timer *timer = new client_timer;
    timer->user_data = &users_timer[connfd];
    
    //获取当前时间
    time_t cur = time(NULL);
    timer->expire = cur + 3 * TIMESLOT;
    users_timer[connfd].timer = timer;

    //添加到链表中
    utils.m_timer_lst.add_timer(timer);
}

void WebServer::dealclientdata()
{
    struct sockaddr_in client_addr;
    epoll_event event;
    socklen_t client_addr_len = sizeof(client_addr);

    //接受连接
    int connfd = accept(m_listenfd, (struct sockaddr *)&client_addr, &client_addr_len);
    judge_error(connfd < 0, "accept error");
    judge_error(http_conn::m_user_count >= MAX_FD, "超过最大数量");

    // //设置非阻塞
    // utils.setnonblocking(connfd);

    // //把新的cfd加入树中
    // event.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLET ;
    // event.data.fd = connfd;
    // int ret = epoll_ctl(m_epollfd, EPOLL_CTL_ADD, connfd, &event);
    // judge_error(ret < 0, "epoll_ctl失败");

    //初始化http对象
    users[connfd].init(connfd, client_addr,m_root);
    //定时器
    timer(connfd, client_addr);
}

//处理信号
bool WebServer::dealwithsignal(bool& timeout, bool& stop_server)
{
    char signals[1024];
    int ret = recv(m_pipefd[0], signals, sizeof(signals), 0);
    if(ret <= 0)
    {
        return false;
    }
    else
    {
        for(int i = 0; i < ret; i++)
        {
            switch(signals[i])
            {
            case SIGALRM:
                timeout = true;
                break;
            case SIGTERM:
                stop_server = true;
                break;
            }
        }
    }
    return true;
}
//若有数据传输，则将定时器往后延迟3个单位
//并对新的定时器在链表上的位置进行调整
void WebServer::adjust_timer(client_timer *timer)
{
    time_t cur = time(NULL);
    timer->expire = cur + 3 * TIMESLOT;
    utils.m_timer_lst.adjust_timer(timer);
}
void WebServer::dealwithread(int sockfd)
{
    client_timer *timer = users_timer[sockfd].timer;
    //有数据传输，定时器延迟
    adjust_timer(timer);
    m_pool->append(users + sockfd, 0);

    //监测到读事件，将该事件放入请求队列
    //m_pool->append(sockfd, 0);

    // char buf[1024];
    // //MSG_WAITALL：等待所有请求的数据到达后再返回。
    // int len = recv(sockfd, buf, sizeof(buf), 0);
    // judge_error(len == -1, "read error");
    // if(len == 0) //说明客户端关闭了连接
    // {
    //     epoll_ctl(m_epollfd, EPOLL_CTL_DEL, sockfd, NULL);
    //     close(sockfd);
    // }
    // else
    // {
    //     char client_buff[1024];
    //     sprintf(client_buff, "server say receive your data : %s\n", buf); 
    //     send(sockfd, client_buff, strlen(client_buff), 0);
        
    //     char ser_buff[1024];
    //     sprintf(ser_buff, "fd : %d, send : %s\n", sockfd, buf);
    //     write(STDOUT_FILENO, ser_buff, strlen(ser_buff));
    // }

}
void WebServer::dealwithwrite(int sockfd)
{
    client_timer *timer = users_timer[sockfd].timer;
    if(timer)
    {
        adjust_timer(timer);
    }
    m_pool->append(users + sockfd, 1);
}
void WebServer::deal_timer(client_timer *timer, int sockfd)
{
    timer->close_sockfd(m_epollfd, &users_timer[sockfd]);
    if(timer)
    {
        utils.m_timer_lst.del_timer(timer);
    }
}

void WebServer::eventListen()
{
    m_listenfd = socket(AF_INET, SOCK_STREAM, 0);
    judge_error(m_listenfd < 0, "创建socket失败");

    //设置延迟关闭，保证关闭后剩下数据完成传输
    struct linger tmp = {1, 1};
    setsockopt(m_listenfd, SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));
    
    //服务器套接字地址结构定义并初始化
    struct sockaddr_in server_addr,client_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(m_port);

    //设置端口复用
    int flag = 1;
    setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

    int ret = bind(m_listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    judge_error(ret < 0, "bind端口绑定失败");

    ret = listen(m_listenfd, 128);
    judge_error(ret < 0, "listen设定监听数失败");

    utils.init(TIMESLOT);

    //epoll创建内核事件表
    epoll_event event;
    m_epollfd = epoll_create(5);
    judge_error(m_epollfd == -1, "epoll_create失败");

    event.events = EPOLLIN;
    event.data.fd = m_listenfd;

    ret = epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_listenfd, &event);
    judge_error(ret < 0, "epoll_ctl失败");

    ret = socketpair(PF_UNIX, SOCK_STREAM, 0, m_pipefd);
    judge_error(ret < 0, "socketpair失败");

    //将写端设置为非堵塞
    utils.setnonblocking(m_pipefd[1]);

    //将读端加入监听
    event.events = EPOLLIN;
    event.data.fd = m_pipefd[0];
    epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_pipefd[0], &event);

    Utils::utils_pipe = m_pipefd;
    utils.addsig(SIGPIPE, SIG_IGN);
    utils.addsig(SIGALRM, utils.sig_handler);
    utils.addsig(SIGTERM, utils.sig_handler);

    //设置定时器
    alarm(TIMESLOT);

    http_conn::m_epollfd = m_epollfd;

}
void WebServer::eventLoop()
{
    bool timeout = false;
    bool stop_server = false;
    while(!stop_server)
    {
        int wait_ret = epoll_wait(m_epollfd,events,1000,-1);
        //sockfd用来接收满足事件的fd
        int sockfd;
        for(int i = 0; i < wait_ret; i++)
        {
            sockfd = events[i].data.fd;
            //如果等于m_listenfd，那就说明有客户端来连接，
            if(sockfd == m_listenfd)
            {
                dealclientdata();
            }
            else if(events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
            {
                //关闭连接，移除对应的定时器
                client_timer *timer = users_timer[sockfd].timer;
                deal_timer(timer,sockfd);
            }
            //处理信号
            else if((sockfd == m_pipefd[0]) && (events[i].events & EPOLLIN))
            {
                bool flag = dealwithsignal(timeout, stop_server);
            }
            //处理客户端发送的数据
            else if(events[i].events & EPOLLIN) 
            {
                dealwithread(sockfd);
            }
            else if(events[i].events & EPOLLOUT)
            {
                dealwithwrite(sockfd);
            }
        }
        //受到信号，查看是否有超时的客户端
        if(timeout)
        {
            utils.timer_handler(m_epollfd);
            timeout = false;
        }
    }
}
