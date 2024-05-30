#ifndef LOCKER_H
#define LOCKER_H

#include "../header.h"
//信息量
class sem
{
public:
    sem();
    sem(int num);
    ~sem();
    void wait();  //调用成功信号量减一
    void post();  //调用成功信号量加一
private:
    sem_t m_sem;
};

//互斥锁
class locker
{
public:
    locker();
    ~locker();
    //加锁
    void lock();
    //解锁
    void unlock();
private:
    pthread_mutex_t m_mutex;   //定义互斥锁
};

#endif