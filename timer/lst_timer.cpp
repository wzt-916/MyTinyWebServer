#include "lst_timer.h"

sort_timer_lst::sort_timer_lst()
{
    head = NULL;
    tail = NULL;
}

sort_timer_lst::~sort_timer_lst()
{
    
}

//调节定时器链表顺序
void sort_timer_lst::adjust_timer(client_timer *timer)
{
    if(!timer)
    {
        return;
    }
    client_timer *node = timer->next;
    if(!node || (timer->expire < node->expire))
    {
        return;
    }
    if(timer == head)
    {
        head = head->next;
        head->prev = NULL;
        timer->next = NULL;
        add_timer(timer, head);
    }
    else
    {
        timer->prev->next = timer->next;
        timer->next->prev = timer->prev;
        add_timer(timer, timer->next);
    }
}

//查看客户端是否超时
void sort_timer_lst::tick(int m_eppollfd)
{
    if(!head)
    {
        return;
    }
    time_t cur = time(NULL);
    client_timer *node = head;
    while(node)
    {
        if(cur < node->expire)
        {
            break;
        }
        node->close_sockfd(m_eppollfd, node->user_data);
        head = node->next;
        if(head)
        {
            head->prev = NULL;
        }
        delete node;
        node = head;
    }
}

void client_timer::close_sockfd(int m_eppollfd, client_data *user_data)
{
    epoll_ctl(m_eppollfd, EPOLL_CTL_DEL, user_data->sockfd, 0);
    char buf[1024];
    sprintf(buf,"客户端:%d 关闭\n",ntohs(user_data->address.sin_port));
    write(STDOUT_FILENO, buf, strlen(buf));
    //关闭套接字
    close(user_data->sockfd);
}

//删除定时器
void sort_timer_lst::del_timer(client_timer *timer)
{
    if(!timer)
    {
        return;
    }
    if((timer == head) && (timer == tail))//说明链表只有一个节点
    {
        head = tail = NULL;
        delete timer;
        return;
    }
    if(timer == head)
    {
        head = head->next;
        head->prev = NULL;
        delete timer;
        return;
    }
    if(timer == tail)
    {
        tail = tail->prev;
        tail->next = NULL;
        delete timer;
        return;
    }
    timer->prev->next = timer->next;
    timer->next->prev = timer->prev;
    delete timer;
}

void sort_timer_lst::add_timer(client_timer *timer, client_timer *head)
{
    client_timer *prev = head;
    client_timer  *tmp = prev->next;
    while (tmp)
    {
        if (timer->expire < tmp->expire)
        {
            prev->next = timer;
            timer->next = tmp;
            tmp->prev = timer;
            timer->prev = prev;
            break;
        }
        prev = tmp;
        tmp = tmp->next;
    }
    if (!tmp)
    {
        prev->next = timer;
        timer->prev = prev;
        timer->next = NULL;
        tail = timer;
    }
}

void sort_timer_lst::add_timer(client_timer *timer)
{
    //如果为空直接返回
    if(!timer)
    {
        return;
    }
    //如果头为空，说明链表为空
    if(head == NULL)
    {
        head = tail = timer;
        return;
    }
    if (timer->expire < head->expire)
    {
        timer->next = head;
        head->prev = timer;
        head = timer;
        return;
    }
    add_timer(timer, head);
}