#pragma once

#include "Logger.h"
#include "NonCopyAble.h"
#include "Buffer.h"
#include "Callback.h"
#include "Socket.h"
#include <string>

using std::string;
class EventLoop;
// 异步操作的话  需要从类内部获得智能指针 所以继承enable_shared_from_this
class TcpConnection : noncopyable,
                    public std::enable_shared_from_this<TcpConnection>{
public:
    TcpConnection(EventLoop *loop,const string &name,
            int sockfd,
            const InetAddress &localAddr,
            const InetAddress &peerAddr);
    ~TcpConnection();

    // 设置/得到一些信息
    EventLoop *getLoop() const { return _loop; }
    const string& getName() const { return _name; }
    const InetAddress &localAddress() const { return _localAddr; }
    const InetAddress &peerAddress() const { return _peerAddr; }
    bool connected() const { return _state == kConnected; }
    bool getTcpInfo(tcp_info *) const;
    string getTcpInfoString() const;
    // 发送信息
    void send(const void *message, int len); // 发送信息接口  还可以定制成发送其他格式的  void*的话连Int也可以直接发？
    void shutdown();
    void forceClose();
    void forceCloseWithDelay(double seconds);
    void setTcpNoDelay(bool on);
    // 设置可读
    void startRead();
    void stopRead();
    bool isReading() const { return _reading; }
    void setConnCallback(const ConnCallback &cb) { _connCallback = cb; }
    void setCloseConnCallback(const CloseConnCallback &cb) { _closeConnCallback = cb; }
    void setWriteCallback(const WriteCallback &cb) { _writeCallback = cb; }
    void setHighWaterMarkCallback(const HighWaterMarkCallback &cb) { _highWaterMarkCallback = cb; }
    // BUFFER
    Buffer *inputBuffer() { return &_inputBuffer; }
    Buffer *outputBuffer() { return &_outputBuffer; }
    // 连接处理   should be called only once
    void connectEstablished();
    void connectDestroyed();
    

private:
    enum StateConn { kDisConnected, kConnecting, kConnected, kDisConnecting };
    void handleRead(TimeStamp reveiveTime);
    void handleWrite();
    void handleClose();
    void handleError();
    void sendInLoop(const void *message, int len);
    void shutdownInLoop();   // 本线程关闭
    void forceCloseInLoop();  // 不一定在本线程了
    void setState(StateConn s) { _state = s; }
    const char *stateToString() const;  // 得到当前状态


    EventLoop *_loop;  // 分配给子Loop
    const string _name;
    StateConn _state;  //  使用原子/非原子的必要性?
    bool _reading;
    // tcp连接回调的channel  用来接收发送的文件描述符socket
    std::unique_ptr<Socket> _socket;
    std::unique_ptr<Channel> _channel;
    const InetAddress _localAddr;
    const InetAddress _peerAddr;
    // 各种回调
    ConnCallback _connCallback; // 这是新连接回调？ 
    MsgCallback _msgCallback;  // 消息
    WriteCallback _writeCallback;  // 写完
    HighWaterMarkCallback _highWaterMarkCallback;
    CloseConnCallback _closeConnCallback;  // 连接退出
    size_t _highWaterMark;
    Buffer _inputBuffer;
    Buffer _outputBuffer;

    // boost::any 这个可以用于任何类型



    
};