#pragma once

// 多路时间分发器，核心的I/O复用，去封装Epoll

#include "NonCopyAble.h"
#include "Channel.h"
#include "EventLoop.h"
#include "TimeStamp.h"
#include <vector>
#include <unordered_map>

class Poller : noncopyable{
public:
    using ChannelList = std::vector<Channel*>;
    Poller(EventLoop *loop);
    virtual ~Poller();  // 作为基类
    // 用来被epoll或者poll 等实现/继承  这样就可以选择性的调用
    // 纯虚函数跟接口一样
    virtual TimeStamp poll(int timeoutMs, ChannelList *activeChannels) = 0;
    virtual void updateChannel(Channel *channel) = 0;
    virtual void removeChannel(Channel *channel) = 0;
    virtual bool hasChannel(Channel *channel) const;
    
    static Poller * newDefaultPoller (EventLoop *loop);

    void assertInLoopThread() const;
protected:  
    // fd对应上channel
    using ChannelMap = std::unordered_map<int, Channel*>;
    ChannelMap _channels;
private:
    EventLoop *_ownerLoop;
};
