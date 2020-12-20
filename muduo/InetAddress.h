// 对sockaddr_in的封装 ip地址会用这个表示  
// 当然有不同的表示方式，不同函数比如bind,accept接收地址的方式也不是sockaddr，可以强转，这算是兼容以前的函数了
// 用sockaddr_in吧，认准其一即可     条条大路通罗马

#include "CopyAble.h"
#include <netinet/in.h>
#include <strings.h>
#include <string>
#include <arpa/inet.h>

// 包含多种构造函数，去组合ip+port
// 然后又有一些转换函数去发送接收等
class InetAddress : copyable{
public:
    explicit InetAddress(uint16_t port, const std::string ip = "0.0.0.0"){
        bzero(&_addr, sizeof(_addr));
        _addr.sin_family = AF_INET;
        _addr.sin_addr.s_addr = inet_addr(ip.c_str());  // 字符串转换成ip地址
        _addr.sin_port = port;
    }
    explicit InetAddress(const sockaddr_in &addr):_addr(addr){}
    std::string toIp();
    std::string toIpPort();
    uint16_t toPort();
    
private:
    // 目前只要ipv4就行了
    sockaddr_in _addr;
};