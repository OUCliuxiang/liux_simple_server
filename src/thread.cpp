#include <exception>
#include <assert.h>
#include <errno.h>

#include "thread.h"
#include "log.h"

// 当前线程
static thread_local pid_t t_tid = 0;
static thread_local std::string t_thread_name = "UNKNOWN";

Semaphore::Semaphore(uint32_t count) {
    if (sem_init(&m_semaphore, 0, count)){
        FATAL("seminit() semaphore initializing failed.")
        throw std::system_error();
    }
}

Semaphore::~Semaphore() {
    sem_destory(&m_semaphore);
}

void Semaphore::wait() {
    if (sem_wait(&m_semaphore)) {
        FATAL("sem_wait() error.")
        throw std::system_error();
    }
}

void Semaphore::notify() {
    if (sem_post(&m_semaphore)) {
        FATAL("sem_post() error.")
        throw std::system_error();
    }
}

// 封装线程执行需要的数据结构
struct ThreadData {
    using ThreadFunc = Thread::ThreadFunc;
    ThreadFunc m_callback;
    std::string m_name;
    pid_t* m_id;
    Semaphore* m_semaphore

    ThreadData( ThreadFunc func, 
                const std::string& name, 
                pid_t* tid,
                Semaphore* sem):
            m_callback(func),
            m_name(name),
            m_id(tid),
            m_semaphore(sem) {}
    
    void runInThread() {
        *m_id = log::GetThreadId();
        m_id = nullptr; // ？？我擦，我不理解
        m_semaphore -> notify(); // 信号量加一 通知主线程子线程启动成功 
        m_semaphore = nullptr; // ？？我擦，我不理解

    }
}