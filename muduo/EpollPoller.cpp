#include "EpollPoller.h"
#include "EventLoop.h"
#include "Logger.h"
#include "unistd.h"
#include <string.h>

namespace{
    const int kNew = -1; // 新创建的channel未添加
    const int kAdded = 1; // 已经被添加
    const int kDeleted = 2;
}

EpollPoller::EpollPoller(EventLoop *loop)
                        :Poller(loop),
                        _epollfd(epoll_create(kInitEventSize)),
                        _events(kInitEventSize){
    // 默认创建epoll 16大小
    if (_epollfd < 0){
        LogFATAL("EpollPoller create error.");
    }
}

EpollPoller::~EpollPoller(){
    ::close(_epollfd); // 用全局的close 和类的，命名空间的区别开
}

TimeStamp EpollPoller::poll(int timeoutMs, ChannelList *activeChannels){
    // 开始epoll_wait然后对channel操作  更新到activeChannel
    // 这样的话上层只需要调用poll就可以得到活跃的事件 Acceptor掌控连接
    LogINFO("Start to poll.");
    int numEvents = ::epoll_wait(_epollfd,
                                &*_events.begin(),
                                static_cast<int>(_events.size()),
                                timeoutMs);
    int savedErrno = errno;
    TimeStamp now(TimeStamp::now());
    if (numEvents > 0){
        // 处理事件
        LogINFO("%d events happened.",numEvents);
        fillActiveChannels(numEvents, activeChannels);
        if (numEvents == _events.size()){
            _events.resize(_events.size()*2);
        }
    }
    else if (numEvents == 0){
        LogINFO("Nothing happeded.");
    }
    else{
        if (savedErrno != EINTR){ // EINTR
            errno = savedErrno;
            LogFATAL("poll happens to error.");
        }
    }
    return now;
}

// 根据channel的fd去更新事件
void EpollPoller::updateChannel(Channel *channel){
    const int index = channel->index();
    LogINFO("Channel index=%d updating.",index);
    if (index == kNew || index == kDeleted){
        const int fd = channel->fd();
        if (index == kNew){ // 刚注册的
            _channels[fd] = channel;
        }
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else{
        const int fd = channel->fd();
        if (channel->isNoneEvent()){
            update(EPOLL_CTL_DEL, channel);
        }
        else{
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EpollPoller::removeChannel(Channel *channel){
    const int fd = channel->fd();
    const int index = channel->index();
    int stat = _channels.erase(fd);
    if (index == kAdded){
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}

void EpollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels){
    // 把events里面活跃的描述符填写到
    for (int i=0; i<=numEvents; i++){
        Channel *channel = static_cast<Channel*>(_events[i].data.ptr);
        int fd = channel->fd();
        channel->set_revents(_events[i].events);
        activeChannels->push_back(channel);
    }
}

void EpollPoller::update(int op, Channel *channel){
    // op是epoll的事件类型   给channel的fd操作更新下  细化到event
    // 直接对vector中的event操作
    epoll_event event;
    bzero(&event, sizeof(event));
    event.events = channel->events();
    int fd = channel->fd();
    event.data.fd = fd;
    event.data.ptr = channel; // 这个没懂
    int stat = ::epoll_ctl(_epollfd, op, fd, &event);
    if (stat < 0){
        if (op == EPOLL_CTL_DEL){
            LogERROR("Delete fd=%d error",fd);
        }
        else if (op == EPOLL_CTL_MOD){
            LogFATAL("Modify fd=%d error",fd);
        }
    }
}