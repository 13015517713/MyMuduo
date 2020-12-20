#pragma once

#include "CopyAble.h"
#include <stdint.h>   // 定义size_t是int64
#include <string>
#include <time.h>

// 主要为了得到当前时间   然后将当前时间转换为年月日时分秒

class TimeStamp : copyable{
public:
    TimeStamp():_unixTime(0){}
    explicit TimeStamp(time_t time):_unixTime(time){}
     // 方式： 直接调用Now就重新返回一个对象，这个对象就是Now现在的时间戳。 比较轻量。 也不用考虑什么线程啥的。
    static TimeStamp now(){
        return TimeStamp(time(NULL));
    }
    std::string toString();// 当前时间转换成 year-mon-day h-m-s 
private:
    time_t _unixTime;
};