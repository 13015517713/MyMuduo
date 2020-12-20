#include "Poller.h"

using namespace std;

Poller::Poller(EventLoop *loop):_ownerLoop(loop){}

// 这个返回的话还要包含子类。 所以不要在这写，在别的地方写。
// 在基类中包含子类不符合逻辑
// Poller * Poller::newDefaultPoller (EventLoop *loop){
// }


bool Poller::hasChannel(Channel *channel) const{
    auto found = _channels.find(channel->fd());
    if(found == _channels.end()){
        return false;
    }
    else{
        return true;
    }
}

// 必须在某个线程中要不报错  保证一个channel一直由一个线程负责
void Poller::assertInLoopThread() const{

}
