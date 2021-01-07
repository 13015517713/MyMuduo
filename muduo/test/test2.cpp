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
// #include "test.h"
using namespace std;
sem_t t;
 int c = 2;
// extern int c;
static void run2(){
    // cout << c << endl;
    // c = 2;
}
