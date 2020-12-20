#include "Channel.h"
#include "EventLoop.h"
#include "Logger.h"
#include <iostream>
#include <sys/epoll.h>

using namespace std;

// 对于epoll不同操作见 https://www.cnblogs.com/gqtcgq/p/7247112.html 和Poll好像是一样的
const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN|EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;


Channel::Channel(EventLoop *loop, int fd)
    : _loop(loop),
    _fd(fd),
    _events(0),
    _revents(0),
    _index(-1),
    _tied(false){
}

// 里面都是一些断言   自己写的库精简了。
// 析构保证在自己所在线程中，后面再来补这个
Channel::~Channel(){}


void Channel::handelEvent(TimeStamp recvTime){
    if(_tied){
        std::shared_ptr<void> ptr = _tie.lock(); // 判断是否存在
        handleEventWithGuard(recvTime);
    }
    else{
        handleEventWithGuard(recvTime);
    }
    return;
}

void Channel::tie(const shared_ptr<void> &ptr){
    _tie = ptr; // 这个就是多线程中智能指针的使用   可以用_tie来查看ptr还是否存在
    _tied = true;
}

// 调用上层接口把它从监听中拿出来
void Channel::remove(){
    // 
    // _loop->removeChannel();
}

// 调用上层的update更新fd监听
void update(){
    // epoll的ctl上面的方法
}

// 那边监听好返回东西了，我这边进行处理就行了
void Channel::handleEventWithGuard(TimeStamp tim){
    LogINFO("Channel's fd = %d starts to handle _revents = %d", _fd, _revents);
    _eventHanding = true;
    // 1.前面是连接错误的处理
    // 2.后面是处理数据读写
    // 对方disconnect了，且无可读数据
    if ((_revents & EPOLLHUP) && !(_revents & EPOLLIN)){
        LogWARNING("fd = %d Channel::handle_event() EPOLLHUP",_fd);
        if (_closeCallBack) _closeCallBack();
    }
    // 连接发生错误
    if (_revents & EPOLLERR){
        LogWARNING("fd = %d Channel::handle_event() EPOLLERR",_fd);
        if (_errorCallBack) _errorCallBack();
    }
    // 可读
    if ((_revents & (EPOLLIN | EPOLLPRI))){
        if (_readCallBack) _readCallBack(tim);
    }
    // 可写
    if (_revents & EPOLLOUT){
        if (_writeCallBack) _writeCallBack();
    }
    _eventHanding = false;
}