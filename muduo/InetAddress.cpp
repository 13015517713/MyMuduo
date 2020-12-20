#include "InetAddress.h"
#include <iostream>
#include <string>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>

using namespace std;

string InetAddress::toIp(){
    char buf[50];
    ::inet_ntop(AF_INET, &_addr.sin_addr, buf, sizeof(buf));
    return buf;
}
string InetAddress::toIpPort(){
    char buf[50];
    ::inet_ntop(AF_INET, &_addr.sin_addr, buf, sizeof(buf));
    sprintf(buf + strlen(buf), "%u", _addr.sin_port);
    return buf;
}
uint16_t InetAddress::toPort(){
    return _addr.sin_port;
}