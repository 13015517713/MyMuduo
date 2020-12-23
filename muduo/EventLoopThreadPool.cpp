#include "EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, const std::string& name)
                            :_baseLoop(baseLoop),
                            _name(name),
                            _started(false),
                            _numThreads(0),
                            _next(0){
}
EventLoopThreadPool::~EventLoopThreadPool(){
    // 所有Eventloop都是栈对象  所以没必要考虑手动delete
}

// 创建一堆EventThread
void EventLoopThreadPool::start(const ThreadInitCallback& cb){
    _started = true;
    for (int i = 0; i < _numThreads; ++i){
        char buf[_name.size()+32];
        snprintf(buf, sizeof buf, "%s%d", _name.c_str(), i);
        EventLoopThread *t = new EventLoopThread(cb, buf);
        _threads.push_back(std::unique_ptr<EventLoopThread>(t));
        _loops.push_back(t->startLoop());
    }
    // 服务端只有一个用户线程 不然就由上面的去执行
    if (_numThreads == 0 && cb){
        cb(_baseLoop);
    }
}

// 轮询方式找到对应的eventloop
EventLoop* EventLoopThreadPool::getNextLoop(){
    EventLoop *loop = _baseLoop;
    if (_loops.size()){
        loop = _loops[_next];
        ++_next;
        if (static_cast<size_t>(_next) >= _loops.size()){
            _next = 0;
        }
    }
    return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops(){
    return _loops;
}
