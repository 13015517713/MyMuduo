/*
我需要弄懂的几个点：
（1）创建线程传参数&都传了什么
（2）创建线程用lambda表达式 这点 lambda表达式作用这么大吗？
*/

#include <thread>
#include <iostream>
using namespace std;

class Thread{
public:
    int a = 1;
    int b = 2;
    void createThread(){
        std::thread _thread(run,this);
    }
    void run(Thread *t){
        std::cout << "run test." << std::endl;
        std::cout << t->a << std::endl;
    }
};

int main(){
    Thread().createThread();
}