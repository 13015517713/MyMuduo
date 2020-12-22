#pragma once

#include "NonCopyAble.h"
#include "TimeStamp.h"
#include "Channel.h"
#include "Poller.h"
#include "CurrentThread.h"
#include <atomic> 
#include <unistd.h>
#include <vector>
#include <functional>
#include <mutex>

// 把它当成epoll    主要封装了poller和监听的事件(channel) 看成epoll就行
// 主要包含epoll+channels
class EventLoop : noncopyable{
public:
    using Functor = std::function<void()>;
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();
    TimeStamp pollReturnTime() const { return _pollReturnTime; }
    // 不知道啥用
    // int64_t iteration() const { return iteration_; }

    // 当前loop执行cb acceptor在本线程执行
    void runInLoop(Functor cb);
    // cb放在队列中，唤醒loop所在线程执行cb   
    void queueInLoop(Functor cb);
    size_t queueSize() const;
    void wakeup();
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);
    bool hasChannel(Channel *channel);
    bool isInLoopThread() const { return _threadId == CurrentThread::tid(); }
private:
    void handleRead();
    void doPendingFunctors(); // 执行functors
    void printActiveChannels(); //打印活跃的channel

    using ChannelList = std::vector<Channel*>;
    // 标识
    std::atomic<bool> _looping;
    std::atomic<bool> _quit;
    // 判断是否有需要的回调
    std::atomic<bool> _callingPendingFunctors;
    const pid_t _threadId; // poll所在线程id
    TimeStamp _pollReturnTime; // poll返回时间
    // 用智能指针，为了退出时会delete. 不用的话要写在析构里，如果多个线程使用的话还要用shared_ptr
    std::unique_ptr<Poller> _poller;

    int _wakeupFd; // 主线程来一个fd 
    std::unique_ptr<Channel> _wakeupChannel;

    ChannelList _activeChannels; // 活跃的channel
    Channel *_currentActiveChannel;

    std::vector<Functor> _pendingFunctors;
    mutable std::mutex _mutex; // 保护上面的回调列表
    
};
