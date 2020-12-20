#include "NonCopyAble.h"
#include "TimeStamp.h"
#include <string>
#include <functional>
#include <memory>

class EventLoop; // 前置声明   包含头文件也行 不过就包含多了   小很多了

// 可以理解为通道
// 就是对文件描述符的封装  in事件 out事件
// 设置了fd的很多回调
class Channel : noncopyable{
public:
    using EventCallBack = std::function<void()>;
    using ReadEventCallBack = std::function<void(TimeStamp)>;
    // 为什么需要loop ?
    Channel(EventLoop *loop, int fd);
    ~Channel();
    // 得到后回调处理事件方法    见明知意
    void handelEvent(TimeStamp recvTime);
    // 设置不同情况的回调
    void setReadCallBack(ReadEventCallBack cb){
        _readCallBack = std::move(cb);
    }
    void setWriteCallBack(EventCallBack cb){
        _writeCallBack = std::move(cb);
    }
    void setErrorCallBack(EventCallBack cb){
        _errorCallBack = std::move(cb);
    }
    void setCloseCallBack(EventCallBack cb){
        _closeCallBack = std::move(cb);
    }
    
    // 防止channel被外部remove了，还在用
    void tie(const std::shared_ptr<void>& );

    // 设置一些返回值的函数
    int fd() { return _fd; }
    int events() const { return _events; }
    int set_revents(int revt) { _revents = revt; }

    // 对fd设置读写监听
    // 对fd的更新最终要落实到epoll中，epoll_ctl，update去更新下。
    bool isNoneEvent() const { return _events == kNoneEvent; }
    void enableReading() { _events |= kReadEvent; update(); }
    void disableReading() { _events &= ~kReadEvent; update(); }
    void enableWriting() { _events |= kWriteEvent; update(); }
    void disableWrite() { _events &= ~kWriteEvent; update(); }
    void disableAll() { _events = kNoneEvent; update(); }
    bool isWriting() const { return _events & kWriteEvent; } 
    bool isReading() const { return _events & kReadEvent; } 

    int index() { return _index;}
    void set_index(int index) { _index = index; }

    EventLoop *ownerLoop() { return _loop; }
    void remove();

private:
    void update();
    void handleEventWithGuard(TimeStamp);

    // 标识描述符注册的事件
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *_loop;
    const int _fd;   // 管理的socket描述符
    int _events;  //  我要监听的事件
    int _revents; // 返回发生的事件
    int _index; // used by Poller ? 

    std::weak_ptr<void> _tie; // 为了查看生命周期/解决循环引用
    bool _tied;
    bool _eventHanding; 
    bool _addedToLoop;
    // 不同的事件回调
    ReadEventCallBack _readCallBack;
    EventCallBack _writeCallBack;
    EventCallBack _errorCallBack;
    EventCallBack _closeCallBack;
};