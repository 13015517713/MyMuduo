#pragma once

#include "NonCopyAble.h"
#include "TimeStamp.h"
#include <string.h>
#include <stdio.h>
#include <string>
#include <iostream>

#define LOG_MAX_LEN 1024


const char *strerrorOur(int savedErrno);

// 等下把这些
#define LogDEBUG(logFormat, ...) \
    do{ \
        Logger &logger = Logger::getInstance();\
        logger.setLevel(Logger::DEBUG); \
        char msg[LOG_MAX_LEN] = {0}; \
        snprintf(msg, LOG_MAX_LEN, logFormat,##__VA_ARGS__);\
        logger.log(__FILE__,__LINE__,msg);\
    }while(0);

#define LogINFO(logFormat, ...) \
    do{ \
        Logger &logger = Logger::getInstance();\
        logger.setLevel(Logger::INFO); \
        char msg[LOG_MAX_LEN] = {0}; \
        snprintf(msg, LOG_MAX_LEN, logFormat,##__VA_ARGS__);\
        logger.log(msg);\
    }while(0);

#define LogWARNING(logFormat, ...) \
    do{ \
        Logger &logger = Logger::getInstance();\
        logger.setLevel(Logger::WARNING); \
        char msg[LOG_MAX_LEN] = {0}; \
        snprintf(msg, LOG_MAX_LEN, logFormat,##__VA_ARGS__);\
        logger.log(msg);\
    }while(0);
#define LogERROR(logFormat, ...) \
    do{ \
        Logger &logger = Logger::getInstance();\
        logger.setLevel(Logger::ERROR); \
        char msg[LOG_MAX_LEN] = {0}; \
        snprintf(msg, LOG_MAX_LEN, logFormat,##__VA_ARGS__);\
        logger.log(msg);\
    }while(0);

#define LogFATAL(logFormat, ...) \
    do{ \
        Logger &logger = Logger::getInstance();\
        logger.setLevel(Logger::FATAL); \
        char msg[LOG_MAX_LEN] = {0}; \
        snprintf(msg, LOG_MAX_LEN, logFormat,##__VA_ARGS__);\
        logger.log(msg);\
        exit(1);\
    }while(0);

// 简化了muduo库的日志输出   直接输出到缓冲区了。
// 输出到文件的细节省略了，不用那么锱铢必较，那个有点难了。
// 单例模式，然后动态设置Level，然后输出
class Logger : noncopyable{
public:
    // 日志级别
    enum LogLevel{
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        FATAL
    };
    static Logger &getInstance(){
        // 这个还得需要设置成静态的
        static Logger _logger;
        return _logger;
    }
    void setLevel(LogLevel);
    void log(std::string msg);
    void log(std::string, int, std::string msg);
private:
    static Logger _logger;
    LogLevel _level;
    //  目前还没有mutex 可能要等下才封装吧
};
