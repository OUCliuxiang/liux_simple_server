// 单例模式封装

#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include <memory> // std::shared_ptr

namespace liux {

// 单例模式封装类，返回单例裸指针        
// 饿汉模式，实例化之初就将指针初始化，
// 懒汉模式会出错，不是很理解，先写成饿汉
template<class T> 
class Singleton {
public:
    static T* GetInstance() {
        static T v;
        return &v;
    }
};

// 单例智能指针封装类，返回单例智能指针
// 饿汉模式，实例化之初就将智能指针初始化，
// 懒汉模式会出错，不是很理解，先写成饿汉
template<class T> 
class SingletonPtr {
public:
    static std::shared_ptr<T> GetInstance() {
        static std::shared_ptr<T> v(new T());
        return v;
    }

};

} // end namespace liux

#endif
