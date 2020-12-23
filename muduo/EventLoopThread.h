#pragma once

/*
    1.创建线程和EventLoop，对EventLoop和Thread的包装
    2.start即开始了Loop
*/

#include "Thread.h"
#include "CopyAble.h"
// #include "EventLoop.h"
#include <semaphore.h>
#include <functional>

class EventLoop;
// 应该是可以拷贝的吧
class EventLoopThread : copyable{  // 包装那两个  作为栈对象
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback(),
            const std::string &name = std::string());
    ~EventLoopThread();
    EventLoop *startLoop();

private:
    void threadFunc();

    // 被包装的两个
    EventLoop *_loop;
    Thread _thread; 
    bool _exiting;
    sem_t _sem; // 用来初始化_loop的。
    ThreadInitCallback _threadInitCallback;
    std::mutex _mutex;
};