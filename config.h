#ifndef CONFIG_H
#define CONFIG_H
#include "webserver.h"

class Config
{
public:
    Config();
    ~Config();
    
    //获取参数
    void parse_arg(int argc, char *argv[]);

    //端口号
    int PORT;
};

#endif