#include "CurrentThread.h"
#include "unistd.h"
#include "sys/syscall.h"


__thread int CurrentThread::t_cachedTid = 0;
void CurrentThread::cacheTid(){
    if (t_cachedTid == 0){
        t_cachedTid = ::syscall(SYS_gettid);
    }
}