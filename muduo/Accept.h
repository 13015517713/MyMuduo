#pragma once

#include "NonCopyAble.h"
#include "Socket.h"
#include "Channel.h"
#include "InetAddress.h"
#include <functional>

class Channel;
// 包含了一些回调 给poller设置
// 有新的fd了回调读，然后再回调new connection
class Acceptor : noncopyable{
public:
    using NewConnCallback = std::function<void(int sockfd, const InetAddress &addr)>;
    explicit Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reusePort = true);
    ~Acceptor();
    void setNewConnCallback(const NewConnCallback &cb){
        _newCpnnCallback = cb;
    }
    void listen();
    bool listening() const;
private:
    // 这个是给channel用的吧，可读了然后回调新连接
    void handleRead();

    EventLoop *_loop;
    Socket _acceptSock;
    Channel _acceptChannel;
    NewConnCallback _newCpnnCallback;
    bool _listening;
    int _idleFd;  // 文件描述符数量有限的  打开空文件创建一个文件描述符，可以了再Accept
};