// #include "test.h"
#include <iostream>
#include <functional>

using namespace std;
using namespace std::placeholders;

void run(int t){
    cout << t << endl;
}

using Functor = std::function<void(int )>;
int main(){
    Functor cb = std::bind(run, _1);
    cb(1);
}