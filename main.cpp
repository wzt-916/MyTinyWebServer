#include "config.h"

int main(int argc, char *argv[])
{
    //命令行解析
    Config config;
    config.parse_arg(argc, argv);

    WebServer server;
    
    //初始化
    server.init(config.PORT);
    server.eventListen();

    //开启处理连接
    server.eventLoop();

    return 0;
}