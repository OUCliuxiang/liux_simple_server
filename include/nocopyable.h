// 不可复制类的基类    

#ifndef __NOCOPYABLE_H__
#define __NOCOPYABLE_H__

namespace liux {

class Nocopyable {

public:
    // c++11 新特性，显式地要求编译器生成一个默认构造函数
    // 默认构造函数负责无参数构造情况下的成员变量初始化，如果不写构造函数
    // 则编译器会生成一个默认构造，但一旦写了其他构造函数，编译器便不再
    // 生成默认构造，此时需要我们显式地要求编译器生成一个默认构造函数。
    Nocopyable() = default;

    // 默认析构
    ~Nocopyable() = default; 

    // 禁用拷贝构造和赋值构造的方式实现禁止拷贝，c++11 新特性
    Nocopyable(const Nocopyable&) = delete;
    Nocopyable& operator=(const Nocopyable&) = delete;
};

} // end namespace liux

#endif // __NOCOPYABLE_H__