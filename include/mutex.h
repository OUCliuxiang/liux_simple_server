// 信号量，互斥锁，自旋锁，范围锁，读写锁，原子锁的封装

#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <stdint.h> // uint32_t 等重定义整型类型
#include <semaphore.h> // 信号量相关
#include <pthread.h> // 线程相关，包括 lock(), unlock() 加锁解锁

namespace liux {

// 信号量，不可拷贝
class Semaphore: Noncopyable {
public:
    // 信号量值的大小
    Semaphore(uint32_t count = 0);
    ~Semaphore();
    // 获取信号量
    void wait();
    // 释放信号量
    void notify();

private:
    sem_t m_semaphore;
}

// 局部锁模板 
template<T>
struct ScopedLockImpl {
public:
    // 构造即上锁
    ScopedLockImpl(T& mutex): m_mutex(mutex) {
        m_mutex.lock();
        m_locked = true;
    }

    ~ScopedLockImpl() {
        unlock();
    }

    void lock() {
        if (!m_locked){
            m_mutex.lock();
            m_locked = true;
        }
    }

    void unlock() {
        if (m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }

private:
    // 锁
    T& m_mutex;
    //是否已上锁
    bool m_locked; 
};

// 局部读锁模板
template<class T>
struct ReadScopedLockImp {
public:
    ReadScopedLockImp(T& mutex): m_mutex(mutex) {
        m_mutex.rdlock();
        m_locked = true;
    }

    ~ReadScopedLockImp() {
        unlock();
    }

    void lock() {
        if (!m_locked) {
            m_mutex.rdlock();
            m_locked = true;
        }
    }

    void unlock() {
        if (m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }

private:
    T& m_mutex;
    bool m_locked;
}


} // end namespace liux

#endif // __MUTEX_H__