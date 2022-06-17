// 信号量，互斥锁，自旋锁，范围锁，读写锁，原子锁的封装

#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <memory>
#include <stdint.h> // uint32_t 等重定义整型类型
#include <semaphore.h> // 信号量相关
#include <pthread.h>    // 线程相关，包括 lock(), unlock() 加锁解锁, mutex 互斥量
#include <atomic>   // 原子量相关

#include "nocopyable.h"

namespace liux {

// 信号量，不可拷贝
class Semaphore: public Nocopyable {
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
};

// 局部锁模板 
template<class T>
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
struct ReadScopedLockImpl {
public:
    ReadScopedLockImpl(T& mutex): m_mutex(mutex) {
        m_mutex.rdlock();
        m_locked = true;
    }

    ~ReadScopedLockImpl() {
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
};

// 局部写锁模板
template<class T> 
struct WriteScopedLockImpl {
public:
    WriteScopedLockImpl(T& mutex): m_mutex(mutex) {
        m_mutex.wrlock();
        m_locked = true;
    }

    ~WriteScopedLockImpl() {
        unlock();
    }

    void lock() {
        if (!m_locked) {
            m_mutex.wrlock();
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
};

// 互斥量，不可复制            
class Mutex: public Nocopyable {
public:
    // 互斥量局部锁
    using Lock = ScopedLockImpl<Mutex>;
    
    Mutex() {
        pthread_mutex_init(&m_mutex, nullptr);
    }

    ~Mutex() {
        pthread_mutex_destroy(&m_mutex);
    }
    
    void lock() {
        pthread_mutex_lock(&m_mutex);
    }

    void unlock() {
        pthread_mutex_unlock(&m_mutex);
    }

private: 
    // 互斥量
    pthread_mutex_t m_mutex;
};

// 读写互斥量         
class RWMutex: Nocopyable {
public:
    // 局部读锁
    using ReadLock = ReadScopedLockImpl<RWMutex>;
    // 局部写锁 
    using WriteLock= WriteScopedLockImpl<RWMutex>;

    RWMutex() {
        pthread_rwlock_init(&m_lock, nullptr);
    }

    ~RWMutex() {
        pthread_rwlock_destroy(&m_lock);
    }

    void rdlock() {
        pthread_rwlock_rdlock(&m_lock);
    }

    void wrlock() {
        pthread_rwlock_wrlock(&m_lock);
    }

    void unlock() {
        pthread_rwlock_unlock(&m_lock);
    }
private:
    // 读写锁
    pthread_rwlock_t m_lock;
};

// 自旋锁
class SpinLock: Nocopyable {
public:
    using Lock = ScopedLockImpl<SpinLock>;

    SpinLock() {
        pthread_spin_init( &m_lock, 0);
    }

    ~SpinLock() {
        pthread_spin_destroy( &m_lock);
    }

    void lock() {
        pthread_spin_lock(&m_lock);
    }

    void unlock() {
        pthread_spin_unlock(&m_lock);
    }

private:
    pthread_spinlock_t m_lock;
};

// 原子锁 
class CASLock: Nocopyable {
public:
    using Lock = ScopedLockImpl<CASLock>;

    CASLock() {
        m_mutex.clear();
    }

    ~CASLock() {
    }

    void lock() {
        while (std::atomic_flag_test_and_set_explicit(&m_mutex, std::memory_order_acquire));
    }

    void unlock() {
        std::atomic_flag_clear_explicit(&m_mutex, std::memory_order_release);
    }

private:
    // volatile 修饰的变量不被编译器优化，每次使用时从它的地址而不是cpu 寄存器中读取
    // volatile 允许来自非编译器的更改，比如操作系统等。
    // 原子状态
    volatile std::atomic_flag m_mutex;

};

} // end namespace liux

#endif // __MUTEX_H__