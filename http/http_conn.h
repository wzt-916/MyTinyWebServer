#ifndef HTTPCONN_H
#define HTTPCONN_H
#include "../header.h"

class http_conn
{
public:
    static const int READ_BUFFER_SIZE = 2048;
    static const int WRITE_BUFFER_SIZE = 1024;
    static const int FILENAME_LEN = 200;
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
        NO_REQUEST,          //请求不完整或尚未接收到完整的HTTP请求头
        GET_REQUEST,         //成功接收到并解析了一个完整的HTTP GET请求   
        BAD_REQUEST,         //请求格式错误或请求无效
        NO_RESOURCE,         //请求的资源不存在
        FORBIDDEN_REQUEST,   //请求被拒绝
        FILE_REQUEST,        //请求的是一个文件资源
        INTERNAL_ERROR,      //服务器内部错误
        CLOSED_CONNECTION    //客户端已经关闭连接
    };
    //http报文的请求行，头部，内容
    enum CHECK_STATE
    {
        CHECK_STATE_REQUESTLINE = 0,  //请求行
        CHECK_STATE_HEADER,           //请求头部
        CHECK_STATE_CONTENT           //内容
    };
    //行状态
    enum LINE_STATUS
    {
        LINE_OK = 0,  //解析成功
        LINE_BAD,     //解析失败
        LINE_OPEN     //行不完整
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
    void init(int sockfd, const sockaddr_in &addr,char *root);
    //循环读取客户数据，直到无数据可读或对方关闭连接
    bool read_once();
    bool write();
    void close_conn(bool real_close = true);
private:
    void init();
    //从状态机，用于分析出一行内容
    LINE_STATUS parse_line();
    char *get_line(){return m_read_buf + m_start_line;};
    //解析http请求行，获得请求方法，目标url及http版本号
    HTTP_CODE parse_request_line(char *text);
    //解析http请求的一个头部信息
    HTTP_CODE parse_headers(char *text);
    //判断http请求是否被完整读入
    HTTP_CODE parse_content(char *text);
    HTTP_CODE do_request();
    //响应报文内容
    bool add_response(const char *format, ...);
    bool add_content(const char *content);
    bool add_status_line(int status, const char *title);
    bool add_headers(int content_length);
    bool add_content_type();
    bool add_content_length(int content_length);
    bool add_linger();
    bool add_blank_line();
public:
    HTTP_CODE process_read();
    bool process_write(HTTP_CODE ret);
    char m_read_buf[READ_BUFFER_SIZE];    //读缓冲
    long m_read_idx;                      
    long m_checked_idx;
    int m_start_line;
    char m_write_buf[WRITE_BUFFER_SIZE];
    int m_write_idx;
    char m_real_file[FILENAME_LEN];
    CHECK_STATE m_check_state;
    char *m_url;
    char *m_version;
    char *m_host;
    bool m_linger;
    METHOD m_method;
    int cgi;        //是否启用的POST
    long m_content_length;
    char *m_string; //存储请求头数据
    char *doc_root;
    struct stat m_file_stat;
    char *m_file_address;
    struct iovec m_iv[2];
    int m_iv_count;
    int bytes_to_send;
    int bytes_have_send;
};

#endif