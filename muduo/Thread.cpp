#include "Thread.h"
#include "Logger.h"
#include "CurrentThread.h"
#include <string>
#include <assert.h>
#include <semaphore.h>

using namespace std;

Thread::Thread(ThreadFunc func, const string &name)
        :_started(false),
        _joined(false),
        _threadId(0),
        _name(name),
        _func(func){
    setDefaultName(); // 没默认名字默认起个名字
}

// 工作线程需要join，守护线程退出自动回收。 
// 线程只是能其中一种状态。
Thread::~Thread(){
    if (_started && !_joined){
        _thread->detach(); // 变成守护线程
    }
}

// start开了一个线程去处理 也没阻塞 
// 这个库基本都没有阻塞
void Thread::start(){
    assert(!_started);
    _started = true;
    sem_t sem;
    sem_init(&sem, false, 0);
    // 执行一个函数传这么多参数吗
    thread(new thread([&](){
        _threadId = CurrentThread::tid();
        sem_post(&sem); // sem被共享吗？
        _func();
    }));
    thread(new thread(_func,this));
    // 这点没有办法考虑_threadId被初始化没
    sem_wait(&sem);
}
void Thread::join(){
    assert(_started);
    assert(!_joined);
    _joined = true;
    _thread->join();
    // return stat;
}

void Thread::setDefaultName(){
    int num = Thread::_numCreated++;
    if(_name.size() == 0){
        char buf[32];
        snprintf(buf, sizeof buf, "Thread%d", num);
        _name = buf;
    }
}