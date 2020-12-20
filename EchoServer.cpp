#include "EchoServer.h"
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpServer.h>
#include <iostream>
#include <functional>

using namespace std;
using namespace muduo;
using namespace muduo::net;

void EchoServer::onConCall(const TcpConnectionPtr& ptr){
    if(!ptr->connected()){
        cout << "offline" << endl;
        return;
    }
    cout << "new connection" << endl;
    cout << ptr->peerAddress().toIpPort() << endl;
}
void EchoServer::onMsgCall(const TcpConnectionPtr& ptr, Buffer* buf, Timestamp tim){
    string tString = buf->retrieveAllAsString();
    cout << tString << endl;
    
    ptr->send(tString);
}
void EchoServer::run(){
    _tcpServer.start();
    _loop->loop();
}

int main(){
    EventLoop loop;
    InetAddress addr("0.0.0.0",8088);
    EchoServer test(&loop, addr, "echo");
    test.run();
    return 0;
}
