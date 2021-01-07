#pragma once

#include "Logger.h"
#include "NonCopyAble.h"
#include "InetAddress.h"
#include "Acceptor.h"
#include "TcpConnection.h"
#include "Callback.h"
#include <memory.h>
#include <atomic>
#include <map>
#include <functional>

/*
    展现给用户的TcpServer
    主要包含：EventLoop池子，
*/

class EventLoop;
class Buffer;
class EventLoopThreadPool;




class TcpServer : noncopyable{
public:
    enum PortOption{
        TagNoReusePort,
        TagReusePort
    };
    TcpServer(EventLoop *loop,
            const InetAddress &listenAddr,
            const std::string &name,
            PortOption option = TagReusePort);
    ~TcpServer();

    const std::string &ipPort() const { return _ipPort; }
    const std::string &name() const { return _name; }
    EventLoop *getLoop() const { return _loop; }

    void setThreadNum(int numThreads);
    void setThreadInitCallback(const ThreadInitCallback &cb){
        _threadInitCallback = cb;
    }
    std::shared_ptr<EventLoopThreadPool> threadPool() const { return _threadPool; }
    void start();
    
    void setConnectionCallback(const ConnCallback &cb) { _connCallback = cb; }
    void setMsgCallback(const MsgCallback &cb) { _msgCallback = cb; }
    void setWriteCallback(const WriteCallback &cb) { _writeCallback = cb; }

private:
    using ConnectMap = std::map<std::string, TcpConnectionPtr>; 

    void newConnection(int sockfd, const InetAddress &peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);  // 线程安全
    void removeConnectionLoop(const TcpConnectionPtr&);  // 不安全在自己的循环删除

    EventLoop *_loop;
    const std::string _ipPort;
    const std::string _name;
    std::unique_ptr<Acceptor> _acceptor;
    
    std::shared_ptr<EventLoopThreadPool> _threadPool;  
    ConnCallback _connCallback;
    MsgCallback _msgCallback;
    WriteCallback _writeCallback; // 写完回调有必要吗？  写日志？
    ThreadInitCallback _threadInitCallback;
    std::atomic<int> _started;
    int _nextConnId;
    ConnectMap _connMap;
    
};