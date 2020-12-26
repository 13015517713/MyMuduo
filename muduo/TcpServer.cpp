#include "TcpServer.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "EventLoopThread.h"
#include "EventLoopThreadPool.h"
#include <functional>
#include <assert.h>

namespace place = std::placeholders;

namespace defaultFunc{
    //std::function<void (const TcpConnectionPtr &)>;
    void defaultConnectBack(const TcpConnectionPtr &conn){
        //  输出新连接的一些信息
        
    }
    void defaultMsgCallback(const TcpConnectionPtr &conn,
                                Buffer*,
                                TimeStamp){

    }
}

TcpServer::TcpServer(EventLoop *loop,
            const InetAddress &listenAddr,
            const std::string &name,
            PortOption option)
    :_loop(loop),
    _ipPort(listenAddr.toIpPort()),  // 调用const对象的函数，对应函数必须对应const，标识下
    _name(name),
    _acceptor(new Acceptor(loop, listenAddr, option)),
    _threadPool(new EventLoopThreadPool(loop, name)),
    _connCallback(defaultFunc::defaultConnectBack),
    _msgCallback(defaultFunc::defaultMsgCallback),
    _nextConnId(1){
    _acceptor->setNewConnCallback(std::bind(&TcpServer::newConnection, this, place::_1, place::_2));
}

TcpServer::~TcpServer(){
    // 析构的话释放所有连接
    // 当前连接还没写
}

void TcpServer::setThreadNum(int numThreads){
    assert(numThreads >= 0);
    _threadPool->setThreadNum(numThreads);
}

void TcpServer::start(){
    // 开启整个循环
    if (_started == 0){
        _threadPool->start();
        assert(!_acceptor->listening());
        _loop->runInLoop(
            std::bind(&Acceptor::listen, _acceptor.get())
            );
    }
}

void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr){
    // 新连接到来，然后封装成tcpConnection
    // 用智能指针去管理
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn){
    // 删除某个tcp连接
}

void TcpServer::removeConnectionLoop(const TcpConnectionPtr&){
    // 和上面的类似  只不过线程是否安全
}
