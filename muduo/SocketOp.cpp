#include "SocketOp.h"
#include "Logger.h"
#include "unistd.h"

int SocketOp::createNonblockingOrDie(sa_family_t family){
    int sockfd = ::socket(family, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0){
        LogFATAL("socket create errors.");
    }
    return sockfd;
}



void SocketOp::close(int sockfd){
    int stat = ::close(sockfd);
    if (stat<0){
        LogERROR("close sockfd errors.");
    }
}