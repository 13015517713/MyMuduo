#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>

namespace SocketOp{ 
    int createNonblockingOrDie(sa_family_t family);
    void closeOrDie(int sockfd);
    ssize_t readv(int fd, const iovec* iov, int cnt);
}