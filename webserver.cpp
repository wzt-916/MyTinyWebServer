#include "webserver.h"

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

    //epoll创建内核事件表
    epoll_event events[MAX_EVENT_NUMBER],event;
    m_epollfd = epoll_create(5);
    judge_error(m_epollfd == -1, "epoll_create失败");

    event.events = EPOLLIN;
    event.data.fd = m_listenfd;

    ret = epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_listenfd, &event);
    judge_error(ret < 0, "epoll_ctl失败");

    // 循环去epoll_wait进行监听
    int wait_ret,cfd;
    socklen_t  client_addr_len;
    while(1)
    {
        wait_ret = epoll_wait(m_epollfd,events,1000,-1);
        //sockfd用来接收满足事件的fd
        int sockfd;
        for(int i = 0; i < wait_ret; i++)
        {
            sockfd = events[i].data.fd;

            //如果等于m_listenfd，那就说明有客户端来连接，
            if(sockfd == m_listenfd)
            {
                client_addr_len = sizeof(client_addr);
                cfd = accept(m_listenfd,(struct sockaddr *)&client_addr, &client_addr_len);
                char str[INET_ADDRSTRLEN];
                printf("received from %s at PORT %d\n",  
                   inet_ntop(AF_INET, &client_addr.sin_addr, str, sizeof(str)),  
                   ntohs(client_addr.sin_port));  
                //将cfd设置为非阻塞
                int flag = fcntl(cfd, EPOLLET);
                flag |= O_NONBLOCK;
                fcntl(cfd, F_SETFL, flag);

                //把新的cfd加入树中
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = cfd;
                ret = epoll_ctl(m_epollfd, EPOLL_CTL_ADD, cfd, &event);
                judge_error(ret < 0, "epoll_ctl失败");
            }
            else //说明有读事件发生
            {
                char buf[1024];
                int len = recv(sockfd, buf, 5, MSG_WAITALL);
                judge_error(len == -1, "read error");
                if(len == 0) //说明客户端关闭了连接
                {
                    epoll_ctl(m_epollfd, EPOLL_CTL_DEL, sockfd, NULL);
                    close(sockfd);
                }
                else // 读数据
                {
                    char client_buff[1024];
                    sprintf(client_buff, "server say receive your data : %s", buf); 
                    send(sockfd, client_buff, strlen(client_buff), 0);
                    
                    char ser_buff[1024];
                    sprintf(ser_buff, "fd : %d, send : %s", sockfd, buf);
                    write(STDOUT_FILENO, ser_buff, strlen(ser_buff));
                }
            }
        }
    }

}
