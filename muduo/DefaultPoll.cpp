#include "Poller.h"
#include "EpollPoller.h"
#include "PollPoller.h"
#include "EventLoop.h"
#include <stdlib.h>

Poller * Poller::newDefaultPoller (EventLoop *loop){
    if(::getenv("MUDUO_USE_POLL")){  // 判断Epoll使用什么
        return new EpollPoller(loop);
    }
    else{
        return new EpollPoller(loop);
    }
}