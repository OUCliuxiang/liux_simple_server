#ifndef __THREAD_H__
#define __THREAD_H__

#include <functional>
#include <memory>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

#include "noncopyable.h"


// 封装一个原子量
class Semaphore : public noncopyable{
public:
    explicit Semaphore(uint32_t count);
    ~Semaphore();
    // -1，值为零时阻塞
    void wait();
    //  +1
    void notify();

private:
    sem_t m_semaphore;
};

// 封装域线程锁
template<typename T> 
class ScopedLockImpl {
public:
    explicit ScopedLockImpl(T* mutex)
        : m_mutex(mutex)
    {
        m_mutex->lock();
        m_locked = true;
    }

    ~ScopedLockImpl() { unlock(); }

    void lock()
    {
        if (!m_locked)
        {
            m_mutex->lock();
            m_locked = true;
        }
    }

    void unlock()
    {
        if (m_locked)
        {
            m_locked = false;
            m_mutex->unlock();
        }
    }

private:
    T* m_mutex;
    bool m_locked;
};

// 封装一个域读锁
template <typename T>
class ReadScopedLockImpl{
public:
    explicit ReadScopedLockImpl(T* mutex)
        : m_mutex(mutex)
    {
        m_mutex->readLock();
        m_locked = true;
    }

    ~ReadScopedLockImpl() { unlock(); }

    void lock()
    {
        if (!m_locked)
        {
            m_mutex->readLock();
            m_locked = true;
        }
    }

    void unlock()
    {
        if (m_locked)
        {
            m_locked = false;
            m_mutex->unlock();
        }
    }

private:
    T* m_mutex;
    bool m_locked;
};

// 封装一个域写锁
template <typename T>
class WriteScopedLockImpl {
public:
    explicit WriteScopedLockImpl(T* mutex)
        : m_mutex(mutex)
    {
        m_mutex->writeLock();
        m_locked = true;
    }

    ~WriteScopedLockImpl() { unlock(); }

    void lock()
    {
        if (!m_locked)
        {
            m_mutex->writeLock();
            m_locked = true;
        }
    }

    void unlock()
    {
        if (m_locked)
        {
            m_locked = false;
            m_mutex->unlock();
        }
    }

private:
    T* m_mutex;
    bool m_locked;
};

// 封装互斥量
class Mutex {
public:
    Mutex()
    {
        pthread_mutex_init(&m_mutex, nullptr);
    }

    ~Mutex()
    {
        pthread_mutex_destroy(&m_mutex);
    }

    int lock()
    {
        return pthread_mutex_lock(&m_mutex);
    }

    int unlock()
    {
        return pthread_mutex_unlock(&m_mutex);
    }

private:
    pthread_mutex_t m_mutex{};
};

// 封装读写锁     
class RWLock {
public:
    RWLock()
    {
        pthread_rwlock_init(&m_lock, nullptr);
    }

    ~RWLock()
    {
        pthread_rwlock_destroy(&m_lock);
    }

    int readLock()
    {
        return pthread_rwlock_rdlock(&m_lock);
    }

    int writeLock()
    {
        return pthread_rwlock_wrlock(&m_lock);
    }

    int unlock()
    {
        return pthread_rwlock_unlock(&m_lock);
    }

private:
    pthread_rwlock_t m_lock{};
};

using ScopedLock = ScopedLockImpl<Mutex>;
using ReadScopedLock = ReadScopedLockImpl<RWLock>;
using WriteScopedLock = WriteScopedLockImpl<RWLock>;


class Thread: public noncopyable {

public:
    using ptr = std::shared_ptr<Thread>;
    using uptr = std::unique_ptr<Thread>;
    using ThreadFunc = std::function<void()>;

    Thread(ThreadFunc callback, const std::string& name);
    ~Thread();

    // 获取线程 pid 
    inline pid_t getId() const {return m_id;}
    // 获取线程名
    inline const std::string& getName() const {return m_name;}
    // 设置线程名 
    inline void setName(const std::string& name) {m_name = name;}
    // 线程并入主线程 
    int join();

public:
    // 获取当前线程Id 
    static pid_t GetThisId();
    // 获取当前运行线程的名称
    static const std::string& GetThisThreadName();
    // 设置当前运行线程的名称
    static void SetThisThreadName(const std::string& name);
    // 启动线程, 接收 Thread*
    static void* Run(void* arg);

private:
    // 系统线程 id, 通过 syscall() 获取
    pid_t m_id;
    // 线程名称
    std::string m_name;
    // pthread 线程 id
    pthread_t m_thread;
    // 线程执行的函数
    ThreadFunc m_callback;
    // 控制线程启动的信号量
    Semaphore m_semaphore;
    // 线程状态
    bool m_started;
    bool m_joined;
};


#endif // __THREAD_H__