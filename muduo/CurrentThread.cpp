#include "CurrentThread.h"
#include "unistd.h"
#include "sys/syscall.h"

void CurrentThread::cacheTid(){
    if (t_cachedTid == 0){
        t_cachedTid = ::syscall(SYS_gettid);
    }
}