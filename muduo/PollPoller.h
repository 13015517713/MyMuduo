#include "Poller.h"
#include "EventLoop.h"

class PollPoller : Poller{
public:
    PollPoller(EventLoop *loop);
};