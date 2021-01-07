#pragma once

#include "CopyAble.h"
#include <stdint.h>
#include <vector>
#include <assert.h>
#include <string>

// #define sizeT std::size_t
// 针对分包粘包的处理方式    前八字节标识包大小   一个读标记一个写标记
/// A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
///
/// @code
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
/// @endcode

/*
    分析：连接收到消息了，我就写进来，然后读。
    写之前，查看后面的够不够，不够的话再考虑前面预留的合并，加起来
    不够就resize.
    前面固定预留了kCheapPrend长度


    连接的消息,buffer.readFd读进来，然后通过retriveAsString转换成字符串就行了，这是主要作用。
    prepend什么的，应该是加紧急数据，加到前面的，append是加到后面的，这些业务端都用不到。
    这个缓冲区的优点在于：用vector去存储char，用了iovec包含多个缓冲区，用了readindex、writeindex去刻画读写过程，可及时利用上读过信息的空间。
    我觉得可以增加一个读取了但是不删除数据的函数，之后再进行添加吧，没有遇到对应需求。
*/
class Buffer : copyable{
public:
    using sizeT = std::size_t;
    static const sizeT kCheapPrepend = 8; // 数据包长度
    static const sizeT kInitialSize = 1024; // 缓冲区大小

    explicit Buffer(sizeT initialSize = kInitialSize)
                :_buffer(initialSize + kCheapPrepend),
                _readIndex(kCheapPrepend),
                _writeIndex(kCheapPrepend){
        // assert();
    }
    ~Buffer(){}
    sizeT readableBytes() const{
        return _writeIndex - _readIndex;
    }
    sizeT writeableBytes() const{
        return _buffer.size() - _writeIndex;
    }
    sizeT prependableBytes() const{
        return _readIndex;
    }

    // 找到第一个可读起始地址
    const char *peek() const{ 
        return begin() + _readIndex;
    }
    void retrieve(sizeT len){ 
        if (len < readableBytes()){ // 读可读缓冲区一部分
            _readIndex += len;
        }
        else{
            retrieveAll();
        }
    }
    void retrieveAll(){ // 重新置位
        _readIndex = kCheapPrepend;
        _writeIndex = kCheapPrepend;
    }
    std::string retrieveAllAsString(){
        return retrieveAllAsString(readableBytes());
    }
    std::string retrieveAllAsString(sizeT len){
        assert(len <= readableBytes());
        std::string result(peek(), len); //读取下len长度字节
        retrieve(len);
        return result;
    }
    
    void append(const std::string &str){
        append(str.c_str(), str.size());
    }
    void append(const char *data, sizeT len){
        ensureWriteableBytes(len); // 先确保后面够写，然后开始写
        std::copy(data, data + len, beginWrite());
        hasWritten(len);
    }
    void append(const void* data, sizeT len){
        append(static_cast<const char*>(data), len);
    }
    // 确保写空间够用 确保后面有Len大小够写
    void ensureWriteableBytes(sizeT len){
        if (writeableBytes() < len){
            makeSpace(len);
        }
    }
    void hasWritten(sizeT len){
        _writeIndex += len;
    }
    void unwrite(sizeT len){ // 删除某些可读数据
        assert(len <= readableBytes());
        _writeIndex -= len;
    }
    // 把数据插入到前面
    void prepend(const void * data, sizeT len){
        _readIndex -= len;
        const char *d = static_cast<const char*>(data);
        std::copy(d, d+len, begin()+_readIndex);
    }
    sizeT dataSize(){
        return readableBytes();
    }
    // 未实现  这个是把写的收缩下 可用swap实现  
    // 当较难实现的时候，可重新创建对象然后swap，不过对象大耗时
    // 所以不易经常调用
    void shrink(){}

    ssize_t readFd(int fd, int *savedErron);  // 外部主要靠这个去把文件缓冲区的内容读进来
    ssize_t writeFd(int fd, int *savedErron);  // 这个是把自己的可读写到fd中
private:
    char *begin(){
        return &*_buffer.begin();
    }
    const char *begin() const{
        return &*_buffer.begin();
    }
    char *beginWrite(){
        return begin() + _writeIndex;
    }
    void makeSpace(sizeT len){  // 确保写空间够用
        // 1.需要合并
        int lenRead = readableBytes();
        if (prependableBytes() + writeableBytes() >= len + kCheapPrepend){
            std::copy(begin() + _readIndex,
                begin() + _writeIndex,
                begin() + kCheapPrepend);
            _readIndex = kCheapPrepend;
            _writeIndex = lenRead + _readIndex;
        }
        else{
            _buffer.resize(len + _writeIndex);
        }
    }
    std::vector<char> _buffer;
    sizeT _readIndex;
    sizeT _writeIndex;
};