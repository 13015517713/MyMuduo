#pragma once

class copyable{
public:
    copyable(const copyable &) = default;
protected:  //  得让子类可以构造
    copyable()=default;
    ~copyable()=default;
};