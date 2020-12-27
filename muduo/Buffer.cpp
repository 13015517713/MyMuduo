#include "Buffer.h"
#include "SocketOp.h"
#include <errno.h>
#include <sys/uio.h>

const size_t Buffer::kCheapPrepend;
const size_t Buffer::kInitialSize;
const int maxExtraLen = 10;

ssize_t Buffer::readFd(int fd, int *saveError){
    char extrabuf[maxExtraLen];
    iovec vec[2];
    const size_t writeable = writeableBytes();
    vec[0].iov_base = begin()+_writeIndex;
    vec[0].iov_len = writeable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;
    const int iovcnt = (writeable < sizeof extrabuf) ? 2 : 1;
    const ssize_t n = SocketOp::readv(fd, vec, iovcnt);
    if (n < 0){
        *saveError = errno;
    }
    else if (static_cast<size_t>(n) <= writeable){
        _writeIndex += n;
    }
    else{  // extra里面也写了
        _writeIndex = _buffer.size();
        append(extrabuf, n-writeable);
    }
    return n;
}