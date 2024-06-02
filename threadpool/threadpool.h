#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "../header.h"
#include "../lock/locker.h"
template<typename T>
class threadpool
{
public:
    threadpool(int thread_number = 8, int max_request = 10000);
    ~threadpool();
    //将消息添加到消息队列上
    bool append(T *request, int state);
private:
    //工作线程运行的函数，它不断从工作队列中取出任务并执行
    static void *worker(void *arg);
    void run(int name);

private:
    int m_thread_number;          //线程池中的线程数
    int m_max_requests;           //请求队列中允许的最大请求数
    pthread_t *m_threads;         //描述线程池的数组，其大小为m_thread_number
    int name;                     //记录是哪一个线程
    locker m_queuelocker;         //保护请求队列的互斥锁      
    std::list<T *> m_workqueue;   //消息请求队列
    sem m_queuestat;              //信号量,是否有任务需要处理
};

template<typename T>
void *threadpool<T>::worker(void *arg)
{
    threadpool *pool = (threadpool *)arg;
    pool->run(pool->name);
    return pool;
}

template<typename T>
void threadpool<T>::run(int name)
{
    while(true)
    {
        m_queuestat.wait();     //等待是否有任务
        m_queuelocker.lock();   //加锁取任务
        if(m_workqueue.empty())
        {
            m_queuelocker.unlock();
            continue;
        }
        T *request = m_workqueue.front();
        m_workqueue.pop_front();
        printf("线程 %d 领取到任务，开始工作\n", name);
        m_queuelocker.unlock(); 
        if(!request)
        {
            continue;
        }
        //读事件
        if(0 == request->m_state)
        {
            if(request->read_once())
            {
                request->process();
            }
            else
            {

            }
        }
        else  //写事件
        {
            request->write();
        }
        //write(STDOUT_FILENO, request->m_read_buf, strlen(request->m_read_buf));
        printf("线程 %d 工作结束\n", name);
    }
}

template<typename T>
threadpool<T>::threadpool(int thread_number, int max_request)
{
    m_thread_number = thread_number;
    m_max_requests = max_request;
    m_threads = NULL;
    if(thread_number <= 0 || max_request <= 0)
    {
        throw std::exception();
    }
    m_threads = new pthread_t[m_thread_number];
    if(!m_threads)
    {
        throw std::exception();
    }
    for(int i = 0; i < thread_number; i++)
    {
        //创建线程
        name = i;
        int ret = pthread_create(m_threads + i, NULL, worker, this);
        if(ret != 0)
        {
            delete[] m_threads;
            throw std::exception();
        }
        ret = pthread_detach(m_threads[i]);
        if(ret != 0)
        {
            delete[] m_threads;
            throw std::exception();
        }
    }
}

template <typename T>
threadpool<T>::~threadpool()
{
    delete[] m_threads;
}

template <typename T>
bool threadpool<T>::append(T *request, int state)
{
    m_queuelocker.lock();
    if(m_workqueue.size() >= m_max_requests)
    {
        m_queuelocker.unlock();
        return false;
    }
    request->m_state = state;
    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.post();
    return true;
}
#endif