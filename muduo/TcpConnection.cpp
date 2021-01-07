#include "TcpConnection.h"
#include "InetAddress.h"
#include "SocketOp.h"
#include <functional>
// #include 

using namespace std::placeholders;

TcpConnection::TcpConnection(EventLoop *loop,
            const string &name,
            int sockfd,
            const InetAddress &localAddr,
            const InetAddress &peerAddr)
    :_loop(loop),
    _name(name),
    _state(kDisConnected),
    _reading(false),
    _socket(new Socket(sockfd)),
    _channel(new Channel(loop, sockfd)),
    _localAddr(localAddr),
    _peerAddr(peerAddr),
    _highWaterMark(){  // 这是缓冲区大小吗？ 等下分析下
    _channel->setReadCallBack(
        std::bind(&TcpConnection::handleRead, this, _1));
    _channel->setWriteCallBack(
        std::bind(&TcpConnection::handleWrite, this));
    _channel->setCloseCallBack(
        std::bind(&TcpConnection::handleClose, this));
    _channel->setErrorCallBack(
        std::bind(&TcpConnection::handleError, this));
    LogDEBUG("TcpConnection creates for fd=%d.",sockfd);
    _socket->setKeepAlive(true); // 设置保活
    _socket->setReuseAddr(true);
    _socket->setReusePort(true);
}

TcpConnection::~TcpConnection(){
    LogDEBUG("TcpConnection deleted : fd=%d state=%s", _channel->fd(), stateToString());
}

// 读epoll收到的数据
void TcpConnection::handleRead(TimeStamp recTime){
    int savedErrno = 0;
    ssize_t len = _inputBuffer.readFd(_channel->fd(), &savedErrno);
    if (len > 0){
        _msgCallback(shared_from_this(), &_inputBuffer, recTime);
    }
    else if (len == 0){
        handleClose(); 
    }
    else{
        errno = savedErrno;
        LogERROR("TcpConnection handleRead errors.");
        handleError();
    }
}

// 可写有什么用   muduo用的直接写，用output buffer中的数据写。  直接用Outputbuffer的数据吧
void TcpConnection::handleWrite(){
    if (_channel->isWriting()){
        int savedErrno = 0;
        ssize_t len = _outputBuffer.writeFd(_channel->fd(), &savedErrno);  // 可写一定有数据
        if (len > 0){  // 写成功了
            _outputBuffer.retrieve(len);
            if (_outputBuffer.readableBytes() == 0){
                // 写完了
                _channel->disableWrite();
                if (_writeCallback){
                // 虽然确定在自己的线程，但是如果不着急，也可以queueInLoop.先处理事件
                // 返回之后先处理事件，最后再调用仿函数处理回调，其实挺好
                // 事件更重要
                    _loop->queueInLoop(
                        std::bind(_writeCallback, shared_from_this())); 
                }
                if (_state == kDisConnecting){
                    shutdownInLoop();
                }
            }
        }
        else{
            LogFATAL("TcpConnection handleWrites errors.");
        }
    }
    else{
        LogERROR("TcpConnection Fd=%d can't write.", _channel->fd());
    }
}

// 处理关闭
void TcpConnection::handleClose(){ 
    setState(kDisConnected);
    _channel->disableAll();
    TcpConnectionPtr connPtr(shared_from_this());
    _connCallback(connPtr);  // 连接回调   其实可以和关闭连接的回调放在一起
    _closeConnCallback(connPtr);
}

void TcpConnection::handleError(){
    // 处理连接错误，输出error原因  连接错误还能继续读写吗？
    _channel->disableAll();
    int err = SocketOp::getSocketError(_channel->fd());
    LogERROR("TcpConnection name=%s handleError errors for %s", 
            _name, strerrorOur(err));
}


bool TcpConnection::getTcpInfo(tcp_info *tcpPtr) const{
    return _socket->getTcpInfo(tcpPtr);
}

string TcpConnection::getTcpInfoString() const{
    char buf[1024];  // 这不是栈对象吗
    buf[0] = '\0';
    _socket->getTcpInfoToString(buf, sizeof buf);
    return buf;
    // return buf指针返回string类型   string会复制一遍
}

void TcpConnection::send(const void *msg, int len){
    // wait for complete
    if (_state == kConnected){
        // 本线程的连接直接发
        if (_loop->isInLoopThread()){   // 我觉得这个函数懂了很重要
            sendInLoop(msg, len);
        }
        else{
            // 给那个线程的
            void (TcpConnection::*fp)(const void*, int) = &TcpConnection::sendInLoop;
            string s = static_cast<const char *>(msg);
            _loop->runInLoop(
                std::bind(fp, this, msg, len));
        }
    }
}

// 对远端发信息的
void TcpConnection::sendInLoop(const void *msg, int len){
    
}

// 
