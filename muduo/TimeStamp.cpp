#include "TimeStamp.h"
#include <iostream>

using namespace std;

string TimeStamp::toString(){
    tm tmTime;
    localtime_r(&_unixTime, &tmTime);
    char buf[32];
    snprintf(buf, sizeof(buf), "%d-%d-%d %d:%d:%d ",
                        tmTime.tm_year+1900,
                        tmTime.tm_mon+1,
                        tmTime.tm_mday,
                        tmTime.tm_hour,
                        tmTime.tm_min,
                        tmTime.tm_sec
                        );
    return buf;
}
