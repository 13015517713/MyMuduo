#pragma once

#include "CopyAble.h"
#include "InetAddress.h"
// #include <netinet/tcp.h>

/*
    不针对所有的socket，感觉有针对性的accept
    
*/
struct tcp_info; // 包含了tcp的窗口大小等等

class Socket : copyable{
public:
    explicit Socket(int sockfd):_sockfd(sockfd){}
    ~Socket();
    int fd() const { return _sockfd; }
    bool getTcpInfo(struct tcp_info *) const; // 得到fd的信息
    bool getTcpInfoToString(char *buf, int len) const;

    void bindAddress(const InetAddress &addr); // 绑定Ip地址
    void listen();
    // 得到一个连接并且把对端IP地址初始化
    int accept(InetAddress *peeraddr);

    void shutdownWrite();
    void setTcpNoDelay(bool on); // 设置是否开启nagle算法 默认开启但是会有延时
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on); 

private:
    const int _sockfd;  // 对一个描述符操作
};

