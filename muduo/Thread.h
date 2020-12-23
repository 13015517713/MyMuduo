#pragma once

#include "NonCopyAble.h"
#include <thread>
#include <functional>
#include <atomic>

class Thread : noncopyable{
public:
    using ThreadFunc = std::function<void()>;
    Thread(ThreadFunc, const string& name = string());
    ~Thread();

    void start();
    void join();

    bool started() const { return _started; }
    int tid() const { return _threadId; }
    // 为什么返回常引用呢？
    string name() const { return _name; }

    static int numCreated() { return _numCreated; } 
private:
    void setDefaultName();

    bool _started;
    bool _joined;
    int _threadId;
    ThreadFunc _func;
    // 用unique_ptr看行不行
    std::unique_ptr<thread> _thread;
    string _name;
    static atomic<int> _numCreated;

};