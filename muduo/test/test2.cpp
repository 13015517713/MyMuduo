/*
我需要弄懂的几个点：
（1）创建线程传参数&都传了什么
（2）创建线程用lambda表达式 这点 lambda表达式作用这么大吗？
*/
// #include "test.h"
#include <semaphore.h>
#include <thread>
#include <iostream>
#include <unistd.h>
using namespace std;
sem_t t;
void run(){
    sleep(10);
    sem_wait(&t);
    cout << 1 << endl;
}
int main(){
    sem_init(&t, 0 , 0);
    thread _thread(run);
    sem_post(&t);
    cout << "has post" << endl;
    _thread.join();
}