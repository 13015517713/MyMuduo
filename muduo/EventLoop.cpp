#include "EventLoop.h"
#include "Logger.h"
#include "Poller.h"
#include <sys/eventfd.h>
#include <unistd.h>
#include <functional>
#include <bits/std_mutex.h>

namespace{
    // 用来标识此线程唯一的eventloop  不得多个
    __thread EventLoop * t_loopInThisThread = 0;

    const int kPollTimeMs = 10000;

    // 用eventfd的方式去通知其他线程
    // 给Wakeup fd用来通知子反应堆
    int createEventfd(){
        int evtfd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
        if (evtfd < 0){
            LogFATAL("Create eventfd error.");
        }
        return evtfd;
    }

};

EventLoop::EventLoop()
        :_looping(false),
        _quit(false),
        _callingPendingFunctors(false),
        _threadId(CurrentThread::tid()),
        _poller(Poller::newDefaultPoller(this)),
        _wakeupFd(createEventfd()), //用来通知其他线程的 有消息来了快来处理
        _wakeupChannel(new Channel(this, _wakeupFd)),
        _currentActiveChannel(NULL){
    LogINFO("Eventloop created.");
    if (t_loopInThisThread){
        LogFATAL("Other Eventloop id=%d exists this thread.",t_loopInThisThread);
    }
    else{
        t_loopInThisThread = this; 
    }
    // 每一个eventloop对象都监听一个wakeupfd，这样主线程就可以唤醒一个了
    _wakeupChannel->setReadCallBack(
                    std::bind(&EventLoop::handleRead, this));
    _wakeupChannel->enableReading();
}

EventLoop::~EventLoop(){
    // 删除wakeupChannel
    _wakeupChannel->disableAll();
    _wakeupChannel->remove();
    ::close(_wakeupChannel->fd());
    t_loopInThisThread = 0;
}

void EventLoop::handleRead(){
    uint64_t one = 1;
    ssize_t n = ::read(_wakeupFd, &one, sizeof(one));
    // 每一个子反应堆监听了一个wakeupFd
    // 主反应堆随便发下  就可以唤醒一个子反应堆
    if (n != sizeof one){
        LogERROR("EventLoop::handleRead errors.");
    }
}

void EventLoop::wakeup(){
    uint64_t one = 1; 
    ssize_t n = ::write(_wakeupFd, &one, sizeof one);
    if (n != sizeof one){
        LogERROR("EventLoop::wakeup errors.");
    }
}

// 开启poll的监听  然后处理activeChannel
void EventLoop::loop(){
    _looping = true;
    _quit = false;
    LogDEBUG("EventLoop start loop in thread id=%d",_threadId);
    while (!_quit){
        _activeChannels.clear();
        _pollReturnTime = _poller->poll(kPollTimeMs,&_activeChannels); 
        for(Channel *channel : _activeChannels){
            // poller监听事件上报给eventloop，然后处理channel的事件
            channel->handelEvent(_pollReturnTime);
        }
        // 执行当前EventLoop事件需要处理
        // 在主线程给我唤醒的时候 我启动了，要执行主线程给我的回调
        doPendingFunctors();
    }
    LogDEBUG("EventLoop stop loop in thread id=%d",_threadId);
    _looping = false;
}

void EventLoop::quit(){
    _quit = true; // 用来下次不再循环
    if (!isInLoopThread()){ 
        wakeup();
    }
}

// 当前loop执行cb 主线程通知子线程放到子线程队列 子线程直接执行
void EventLoop::runInLoop(Functor cb){
    if (isInLoopThread()){
        cb();
    }
    else{
        queueInLoop(std::move(cb));
    }
}

// cb放在队列中，唤醒loop所在线程执行cb  
void EventLoop::queueInLoop(Functor cb){
    {
        std::lock_guard<std::mutex> tmutex(_mutex);
        _pendingFunctors.push_back(cb);
    }
    if (!isInLoopThread() || _callingPendingFunctors == true){
        wakeup();
    }
}
size_t EventLoop::queueSize() const{
    {
        std::lock_guard<std::mutex> tmutex(_mutex);
        return _pendingFunctors.size();   
    }
}

void EventLoop::updateChannel(Channel *channel){
    _poller->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel){
    _poller->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel *channel){
    return _poller->hasChannel(channel);
}

//  执行functors
void EventLoop::doPendingFunctors(){
    std::vector<Functor> functors;
    _callingPendingFunctors = true;
    {
        std::lock_guard<std::mutex> tmutex(_mutex);
        functors.swap(_pendingFunctors);
    }
    for(const Functor& functor : functors){
        functor();
    }
    _callingPendingFunctors = false;
}

//打印活跃的channel
void EventLoop::printActiveChannels(){
    // print所有channels
}