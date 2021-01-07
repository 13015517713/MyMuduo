#include "Logger.h"
#include "TimeStamp.h"
#include <iostream>
#include <stdio.h>

using namespace std;

__thread char t_errnobuf[512];

void Logger::setLevel(LogLevel level){
    _level = level;
}

const char *strerrorOur(int savedErrno){
    return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
}

void Logger::log(string file, int line, string msg){
    printf("%s %s:%d",TimeStamp::now().toString(), file, line);
    switch (_level){
        case DEBUG:{
            cout << "[DEBUG]:" << msg << endl;
            break;
        }
        case INFO:{
            cout << "[INFO]:" << msg << endl;
            break;
        }
        case WARNING:{
            cout << "[WARNING]:" << msg << endl;
            break;
        }
        case ERROR:{
            cout << "[ERROR]:" << msg << endl;
            break;
        }
        case FATAL:{
            cout << "[FATAL]:" << msg << endl;
            break;
        }
        default:{
            break;
        }
    }
    return;
}



void Logger::log(std::string msg){
    switch (_level){
        case DEBUG:{
            cout << "[DEBUG]:" << msg << endl;
            break;
        }
        case INFO:{
            cout << "[INFO]:" << msg << endl;
            break;
        }
        case WARNING:{
            cout << "[WARNING]:" << msg << endl;
            break;
        }
        case ERROR:{
            cout << "[ERROR]:" << msg << endl;
            break;
        }
        case FATAL:{
            cout << "[FATAL]:" << msg << endl;
            break;
        }
        default:{
            break;
        }
    }
    return;
}
