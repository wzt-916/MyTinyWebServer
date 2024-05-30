#include "locker.h"

locker::locker()
{
    int ret = pthread_mutex_init(&m_mutex, NULL);
    if(ret != 0)
    {
        throw std::exception();
    }
}

locker::~locker()
{
    pthread_mutex_destroy(&m_mutex);
}

void locker::lock()
{
    pthread_mutex_lock(&m_mutex);
}

void locker::unlock()
{
    pthread_mutex_unlock(&m_mutex);
}

sem::sem()
{
    int ret = sem_init(&m_sem, 0, 0);
    if(ret != 0)
    {
        throw std::exception();
    }
}
sem::sem(int num)
{
    int ret = sem_init(&m_sem, 0, num);
    if(ret != 0)
    {
        throw std::exception();
    }
}
sem::~sem()
{
    sem_destroy(&m_sem);
}
void sem::wait()
{
    sem_wait(&m_sem);
}
void sem::post()
{
    sem_post(&m_sem);
}