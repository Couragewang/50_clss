#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <iostream>
#include <queue>
#include <pthread.h>

typedef void (*handler_t)(int);
class Task{
    private:
        int sock;
        handler_t handler;
    public:
        Task()
        {

        }
        void InitTask(int sock_, handler_t h_)
        {
            sock = sock_;
            handler = h_;
        }
        void Run()
        {
            handler(sock);
        }
        ~Task()
        {

        }
};

class ThreadPool{
    private:
        int nums;
        int idle_nums;
        std::queue<Task> task_queue;
        pthread_mutex_t lock;
        pthread_cond_t cond;
    public:
        ThreadPool(int nums_ = 5):nums(nums_),idle_nums(0)
        {
            pthread_mutex_init(&lock, NULL);
            pthread_cond_init(&cond, NULL);
        }
        void LockQueue()
        {
            pthread_mutex_lock(&lock);
        }
        void UnlockQueue()
        {
            pthread_mutex_unlock(&lock);
        }
        void WakeupOneThread()
        {
            pthread_cond_signal(&cond);
        }
        void IdleThread()
        {
            pthread_cond_wait(&cond, &lock);
        }
        bool IsEmpty()
        {
            return task_queue.size() == 0 ? true : false;
        }
        void PushTask(const Task &t)
        {
            LockQueue();
            task_queue.push(t);
            WakeupOneThread();
            UnlockQueue();
        }
        void PopTask(Task &t)
        {
            t = task_queue.front();
            task_queue.pop();
        }
        static void *ThreadRountie(void *arg)
        {
            pthread_detach(pthread_self());
            ThreadPool *tp = (ThreadPool*)arg;
            for( ; ; ){
                tp->LockQueue();
                while(tp->IsEmpty()){
                    tp->IdleThread();
                }
                Task t;
                tp->PopTask(t);
                tp->UnlockQueue();
                t.Run();
            }
        }
        void InitThreadPool()
        {
            pthread_t tid;
            for(auto i = 0; i < nums; i++){
                pthread_create(&tid, NULL, ThreadRountie, (void *)this);
            }
        }
        ~ThreadPool()
        {
            pthread_mutex_destroy(&lock);
            pthread_cond_destroy(&cond);
        }
};

#endif

