#pragma once

class noncopyable{
public:
    noncopyable(const noncopyable &) = delete;
    void operator=(const noncopyable &) = delete;
protected:  //  得让子类可以构造
    noncopyable()=default;
    ~noncopyable()=default;
};