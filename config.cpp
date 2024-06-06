#include "config.h"

Config::Config()
{
    //端口号，默认8080
    PORT = 8080;

    //数据库连接池数量,默认8
    sql_num = 8;

    //线程池内的线程数量,默认8
    thread_num = 8;
}

void Config::parse_arg(int argc, char *argv[])
{
    int opt;
    const char *str = "p:";
    while((opt = getopt(argc, argv, str)) != -1)
    {
        switch(opt)
        {
        case 'p':
            PORT = atoi(optarg);
            break;
        case 's':
            sql_num = atoi(optarg);
            break;
        case 't':
            thread_num = atoi(optarg);
            break;
        default:
            perror("输入参数错误");
            exit(1);
            break;
        }
    }
}

Config::~Config()
{

}
