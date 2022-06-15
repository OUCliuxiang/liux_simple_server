// 单例模式封装

#ifndef __SYLAY_SINGLETON_H__
#define __SYLAY_SINGLETON_H__

#include <memory> // std::shared_ptr

namespace liux {

// 单例模式封装类，返回单例裸指针        
template<class T> 
class Singleton {
public:
    static T* GetInstance() {
        static T v;
        return &v;
    }
};

// 单例智能指针封装类，返回单例智能指针
template<class T> 
class SingletonPtr {
public:
    static std::shared<T>T GetInstance() {
        static std::shared_ptr<T> v(new T);
        return v;
    }
};

}

#endif