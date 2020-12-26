#include "Logger.h"
#include "NonCopyAble.h"
#include "InetAddress.h"
#include "Acceptor.h"
#include "TcpConnection.h"
#include <memory.h>
#include <atomic>
#include <map>

/*
    展现给用户的TcpServer
    主要包含：EventLoop池子，
*/

class EventLoop;
class Buffer;
class EventLoopThreadPool;
class TcpServer : noncopyable{
public:
    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
    using ConnCallback = std::function<void (const TcpConnectionPtr &)>; // 新连接
    using CloseConnCallback = std::function<void (const TcpConnectionPtr &)>; // 连接关闭回调
    // 我完全不理解写回调的含义   打个log倒是可以
    using WriteCallback = std::function<void (const TcpConnectionPtr&)> ;   
    using ThreadInitCallback = std::function<void (EventLoop*)>;
    // using WriteCallback = std::function<void (const TcpConnectionPtr&)>;
    using MsgCallback = std::function<void (const TcpConnectionPtr&,
                                Buffer*,
                                TimeStamp)>;

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