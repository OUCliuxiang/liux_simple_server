#ifndef __SINGLETON_H__
#define __SINGLETON_H__

// 封装一个单例模板类，使用 Meyer's Singleton 风格实现 

#include <memory> 

// 返回实例引用
template <class T> 
class Singleton final { // final 方法修饰，不允许继承 
public:
    static T& get_instance() {
        static T ins;
        return ins;
    }
    Singleton(const Singleton& _ins) = delete;
    Singleton& operator=(const Singleton&) = delete;

private:
    Singleton() = default;
};

// 返回智能指针
template <class T> 
class SingletonPtr final { // final 方法修饰，不允许继承 
public:
    static std::shared_ptr<T> get_instance() {
        static std::shared_ptr<T> ins = std::make_shared<T>();
        return ins;
    }
    SingletonPtr(const SingletonPtr& _ins) = delete;
    SingletonPtr& operator=(const SingletonPtr&) = delete;

private:
    SingletonPtr() = default;
};



#endif // __SINGLETON_H__