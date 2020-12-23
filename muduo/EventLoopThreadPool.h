#include "EventLoop.h"
#include "EventLoopThread.h"
#include "NonCopyAble.h"
#include <string>
#include <vector>
#include <functional>
#include <memory.h>

class EventLoopThreadPool : noncopyable{
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;
    EventLoopThreadPool(EventLoop* baseLoop, const std::string& name);
    ~EventLoopThreadPool();
    void setThreadNum(int numThreads) { _numThreads = numThreads; }
    void start(const ThreadInitCallback& cb = ThreadInitCallback());
    // 轮询算法
    EventLoop* getNextLoop();
    
    std::vector<EventLoop*> getAllLoops();
    bool started() const { return _started; }


private:
    EventLoop *_baseLoop; // 用户线程
    std::string _name;
    bool _started;
    int _numThreads;
    int _next = 0; // 用来轮询子线程
    std::vector<std::unique_ptr<EventLoopThread>> _threads;
    std::vector<EventLoop*> _loops;
};
