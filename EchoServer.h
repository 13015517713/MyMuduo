#ifndef _ECHO_SERVER_H
#define _ECHO_SERVER_H

#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpServer.h>
#include <iostream>
#include <functional>

using namespace muduo;
using namespace muduo::net;
using namespace std::placeholders;
class EchoServer{
public:
    EchoServer(EventLoop *loop,const InetAddress &addr,const string &name):
                               _tcpServer(loop,addr,name),
                               _loop(loop) {
        _tcpServer.setConnectionCallback(std::bind(&EchoServer::onConCall,this,_1));  // bind为了和function契合
        _tcpServer.setMessageCallback(std::bind(&EchoServer::onMsgCall,this,_1,_2,_3));
    }
    void run();
private:
    void onConCall(const TcpConnectionPtr&);
    void onMsgCall(const TcpConnectionPtr&, Buffer*, Timestamp);
    EventLoop *_loop;
    TcpServer _tcpServer;
};

#endif