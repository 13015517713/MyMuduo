#include "Poller.h"
#include <vector>
#include <sys/epoll.h>

/*
    epoll: ctl(ADD,MOD,DEL) create wait
*/

// 继承Poller然后实现poll update remove三个方法
// 首先实现基类必须实现的，然后自己再扩展实现一些该类特有的
class EpollPoller : public Poller{
public:
    EpollPoller(EventLoop *loop);
    ~EpollPoller() override; // override显示指定我要覆盖，如果没覆盖就报错

    TimeStamp poll(int timeoutMs, ChannelList *activeChannels) override;
    void updateChannel(Channel *channel) override;
    void removeChannel(Channel *channel) override;
private:
    void fillActiveChannels(int numEvents, ChannelList *activeChannels);
    void update(int op, Channel *channel);
    static const int kInitEventSize = 16;
    using EventList = std::vector<epoll_event>;
    int _epollfd;
    EventList _events; // 每个描述符对应一个时间
};