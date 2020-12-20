
namespace CurrentThread{
    extern __thread int t_cachedTid;
    void cacheTid();
    // extern __thread char t_tidString[32];
    inline int tid(){
        if (t_cachedTid == 0){
            cacheTid();
        }
        return t_cachedTid;
    }
}