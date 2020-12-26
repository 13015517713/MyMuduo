#include "Acceptor.h"
#include "EventLoop.h"
#include "SocketOp.h"
#include "InetAddress.h"
#include "SocketOp.h"
#include "unistd.h"
#include "fcntl.h"
#include <functional>

Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reusePort)
                :_loop(loop),
                _acceptSock(SocketOp::createNonblockingOrDie(listenAddr.getSockAddr()->sin_family)),
                _listening(false),
                _acceptChannel(loop, _acceptSock.fd()),
                _idleFd(::open("/dev/null", O_RDONLY | O_CLOEXEC)){
    _acceptSock.setReuseAddr(true);
    _acceptSock.setReusePort(true);
    _acceptSock.setKeepAlive(true);
    _acceptSock.setTcpNoDelay(true); // 打开nagle算法
    _acceptChannel.setReadCallBack(
                    std::bind(&Acceptor::handleRead,this));

}

Acceptor::~Acceptor(){
    _acceptChannel.disableAll();
    _acceptChannel.remove();
    ::close(_idleFd);
}

void Acceptor::listen(){
    _acceptSock.listen();
    _listening = true;
    _acceptChannel.enableReading();  // 标记开始可读
}

bool Acceptor::listening() const{
    return _listening;
}

// 这个是给channel用的吧，可读了然后回调新连接  没错
void Acceptor::handleRead(){  // 读到一个文件描述符
    // 给tcpconn用吧
    InetAddress peerAddr;
    int sockfd = _acceptSock.accept(&peerAddr);
    if (sockfd >= 0){  // 可读然后回调新连接 
        if (_newCpnnCallback){
            _newCpnnCallback(sockfd, peerAddr);
        }
        else{  // 感觉没有新连接回调也没事 如果我不需要知道是谁发的    但是不符合工业用的
            SocketOp::closeOrDie(sockfd);
        }
    }
    else{  // 没有反应就不会去accept  去accept就是发生了可读事件
        if (errno == EMFILE){ //文件描述符用满了
            ::close(_idleFd);
            _idleFd = ::accept(_idleFd, NULL, NULL);
            ::close(_idleFd);
            _idleFd = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }
}