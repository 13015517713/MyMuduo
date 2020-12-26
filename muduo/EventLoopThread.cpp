#include "EventLoopThread.h"
#include "EventLoop.h"

EventLoopThread::EventLoopThread(const ThreadInitCallback &cb,
            const std::string &name)
            :_threadInitCallback(cb),
            _loop(NULL),
            _thread(std::bind(&EventLoopThread::threadFunc,this),name),
            _exiting(false)
            {
    sem_init(&_sem, 0, 0);
}

EventLoopThread::~EventLoopThread(){}

// 执行threadFunc 初始化loop
EventLoop *EventLoopThread::startLoop(){
    _thread.start();
    sem_wait(&_sem);
    return _loop;
}

// 开启线程执行循环
void EventLoopThread::threadFunc(){
    EventLoop loop;
    if (_threadInitCallback){
        _threadInitCallback(&loop);
    }
    _loop = &loop;
    sem_post(&_sem);
    loop.loop();
    {
        std::lock_guard<std::mutex> tmutex(_mutex);
        _loop = NULL;
    }
}