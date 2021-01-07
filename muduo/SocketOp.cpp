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



void SocketOp::closeOrDie(int sockfd){
    int stat = ::close(sockfd);
    if (stat<0){
        LogERROR("close sockfd errors.");
    }
}

ssize_t SocketOp::readv(int fd, const iovec* iov, int cnt){
    return ::readv(fd, iov, cnt);
}

int SocketOp::getSocketError(int fd){
    int err;
    socklen_t len = static_cast<socklen_t>(sizeof err);
    if (::getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &len) < 0){
        return errno;
    }
    else{
        return err;
    }
}