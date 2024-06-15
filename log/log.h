#ifndef LOG_H
#define LOG_H

#include "../header.h"
#include "../lock/locker.h"

class Log
{
public:
    static Log *get_instance()
    {
        static Log instance;
        return &instance;
    }
    bool init(const char *file_name, int log_buf_size = 8192, int split_lines = 5000000, int max_queue_size = 0);

    void write_log(int level, const char *format, ...);

    void flush(void);
private:
    Log();
    virtual ~Log();
    char dir_name[128];     //路径名
    char log_name[128];     //log文件名
    int m_log_buf_size;     //日志缓冲区大小
    char *m_buf;
    long long m_count;      //日志行数记录
    int m_split_lines;      //日志最大行数
    int m_today;            //因为按天分类,记录当前时间是那一天
    FILE *m_fp;             //打开log的文件指针
    locker m_mutex;
};
#define LOG_DEBUG(format, ...){ Log::get_instance()->write_log(0, format, ##__VA_ARGS__); Log::get_instance()->flush();}
#define LOG_INFO(format, ...){ Log::get_instance()->write_log(1, format, ##__VA_ARGS__); Log::get_instance()->flush();}
#define LOG_WARN(format, ...){ Log::get_instance()->write_log(2, format, ##__VA_ARGS__); Log::get_instance()->flush();}
#define LOG_ERROR(format, ...){ Log::get_instance()->write_log(3, format, ##__VA_ARGS__); Log::get_instance()->flush();}

#endif