#pragma once

/* 
我需要弄懂的几个点：
（1）创建线程传参数&都传了什么
（2）创建线程用lambda表达式 这点 lambda表达式作用这么大吗？
*/
#include <thread>
#include <iostream>
#include <sys/syscall.h>
using namespace std;

// static int t;
class Thread;
void run(Thread *t);
class Thread{
public:
    int a = 1;
    int b = 2;
    void createThread(){
        int qq = 0;
        cout << ::this_thread::get_id() << endl;
        // std::thread _thread(run,this);
        std::thread _thread([&](){
            cout << a << endl;
            cout << qq << endl;
            cout << ::this_thread::get_id() << endl;
            run(this);
        });
        _thread.join();
    }  
    void c(){
        cout << 1 << endl;
    }
};
