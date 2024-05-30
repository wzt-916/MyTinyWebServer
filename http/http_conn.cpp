#include "http_conn.h"

int http_conn::m_user_count = 0;
int http_conn::m_epollfd = -1;

void http_conn::process()
{
    HTTP_CODE read_ret = process_read();
}
void http_conn::init(int sockfd, const sockaddr_in &addr)
{
    m_sockfd = sockfd;
    m_address = addr;

    char str[INET_ADDRSTRLEN];
    printf("received from %s at PORT %d\n",  
        inet_ntop(AF_INET, &m_address.sin_addr, str, sizeof(str)),  
        ntohs(m_address.sin_port));  
    
    init();
}
//初始化新接受的连接
void http_conn::init()
{
    m_read_idx = 0;

    memset(m_read_buf, '\0', READ_BUFFER_SIZE);
}
http_conn::HTTP_CODE http_conn::process_read()
{

}
//循环读取客户数据，直到无数据可读或对方关闭连接
//非阻塞ET工作模式下，需要一次性将数据读完
bool http_conn::read_once()
{
    if(m_read_idx >= READ_BUFFER_SIZE)
    {
        return false;
    }
    int bytes_read = 0;
    while(true)
    {
        //由于sock是非阻塞的，所以recv也是非阻塞
        bytes_read = recv(m_sockfd, m_read_buf + m_read_idx, READ_BUFFER_SIZE - m_read_idx, 0);
        if(bytes_read == -1)
        {
            //没有数据可接收
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {
                break;
            }
            return false;
        }
        else if(bytes_read == 0) //客户端关闭
        {
            return false;
        }
        m_read_idx += bytes_read;
    }
    return true;
}