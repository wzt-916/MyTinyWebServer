#ifndef HTTPCONN_H
#define HTTPCONN_H
#include "../header.h"

class http_conn
{
public:
    static const int READ_BUFFER_SIZE = 2048;
    //HTTP请求方法类型
    enum METHOD
    {
        GET = 0,
        POST,
        HEAD,
        PUT,
        DELETE,
        TRACE,
        OPTIONS,
        CONNECT,
        PATH
    };
    //HTTP状态码
    enum HTTP_CODE
    {
        NO_REQUEST,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURCE,
        FORBIDDEN_REQUEST,
        FILE_RUQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION
    };
    int m_sockfd;            //文件描述符
    sockaddr_in m_address;   //客户端地址结构
    static int m_epollfd;    //记录epoll
    static int m_user_count; //记录连接的客户端数量
    int m_state;             //读为0，写为1
public:
    //http_conn();
    //~http_conn();
    
    //线程首先调用
    void process();
    //初始化
    void init(int sockfd, const sockaddr_in &addr);
    //循环读取客户数据，直到无数据可读或对方关闭连接
    bool read_once();
private:
    void init();
public:
    HTTP_CODE process_read();
    char m_read_buf[READ_BUFFER_SIZE];    //读缓冲
    long m_read_idx;          
};

#endif