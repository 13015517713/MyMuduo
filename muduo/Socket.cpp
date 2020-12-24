#include "Socket.h"
#include "InetAddress.h"
#include "Logger.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>

Socket::~Socket(){
    ::close(_sockfd);
}

bool Socket::getTcpInfo(struct tcp_info *ptr) const{
    // 得到fd的信息
    socklen_t len = sizeof(*ptr);
    bzero(ptr, len);
    return ::getsockopt(_sockfd, SOL_TCP, TCP_INFO, ptr, &len);
}

bool Socket::getTcpInfoToString(char *buf, int len) const{
    struct tcp_info tcpInfo;
    bool ok = getTcpInfo(&tcpInfo);
    if (ok){
        snprintf(buf, len, "unrecovered=%u "
             "rto=%u ato=%u snd_mss=%u rcv_mss=%u "
             "lost=%u retrans=%u rtt=%u rttvar=%u "
             "sshthresh=%u cwnd=%u total_retrans=%u",
             tcpInfo.tcpi_retransmits,  // Number of unrecovered [RTO] timeouts
             tcpInfo.tcpi_rto,          // Retransmit timeout in usec
             tcpInfo.tcpi_ato,          // Predicted tick of soft clock in usec
             tcpInfo.tcpi_snd_mss,
             tcpInfo.tcpi_rcv_mss,
             tcpInfo.tcpi_lost,         // Lost packets
             tcpInfo.tcpi_retrans,      // Retransmitted packets out
             tcpInfo.tcpi_rtt,          // Smoothed round trip time in usec
             tcpInfo.tcpi_rttvar,       // Medium deviation
             tcpInfo.tcpi_snd_ssthresh,
             tcpInfo.tcpi_snd_cwnd,
             tcpInfo.tcpi_total_retrans);  // Total retransmits for entire connection
    }
    return ok;
}

 // 绑定Ip地址
void Socket::bindAddress(const InetAddress &addr){
    int stat = ::bind(_sockfd, (sockaddr*)addr.getSockAddr(), sizeof sockaddr);
    if(stat<0){
        LogFATAL("Socket bindAddress error.");
    }
}

void Socket::listen(){
    int stat = ::listen(_sockfd, SOMAXCONN);
    if (stat<0){
        LogFATAL("Socket listens error.");
    }
}

// 接收一个连接并且把对端IP地址初始化
int Socket::accept(InetAddress *peeraddr){
    sockaddr_in addr; 
    socklen_t len;
    // 遵循非阻塞的思想  都设置为非阻塞
    // accept4可设置标志位  接收到的文件描述符可直接设置为非阻塞
    // int fd = ::accept4(_sockfd, (sockaddr*) &addr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    // 用accept的：
    int fd = ::accept(_sockfd, (sockaddr*) &addr, &len);
    if ( fd >= 0){
        peeraddr->setSockAddr(addr);
        int stat = ::fcntl(fd, F_SETFL, ::fcntl(fd, F_GETFL, 0)|O_NONBLOCK);
        int stat = ::fcntl(fd, F_SETFD, ::fcntl(fd, F_GETFD, 0)|FD_CLOEXEC);
    }
    peeraddr->setSockAddr(addr);
    return fd;
}

// 关闭socket  关闭写端，读还是要读的。
void Socket::shutdownWrite(){
    int stat = ::shutdown(_sockfd, SHUT_WR);
    if (stat<0){
        LogFATAL("socketFd shutdown errors.")
    }
}

void Socket::setTcpNoDelay(bool on){
    socklen_t confirm = on ? 1 : 0;
    int stat = ::setsockopt(_sockfd, IPPROTO_TCP, TCP_NODELAY,
            &confirm, static_cast<socklen_t>(sizeof confirm));
}

void Socket::setReuseAddr(bool on){
    socklen_t confirm = on ? 1 : 0;
    int stat = ::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR,
            &confirm, static_cast<socklen_t>(sizeof confirm));
}

void Socket::setReusePort(bool on){
    socklen_t confirm = on ? 1 : 0;
    int stat = ::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEPORT,
            &confirm, static_cast<socklen_t>(sizeof confirm));
}

void Socket::setKeepAlive(bool on){
    socklen_t confirm = on ? 1 : 0;
    int stat = ::setsockopt(_sockfd, SOL_SOCKET, SO_KEEPALIVE,
            &confirm, static_cast<socklen_t>(sizeof confirm));
}