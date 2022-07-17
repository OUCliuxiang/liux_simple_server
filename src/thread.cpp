#include <exception>
#include <assert.h>
#include <errno.h>
#include <system_error>

#include "thread.h"
#include "log.h"

// 当前线程
static thread_local pid_t t_tid = 0;
static thread_local std::string t_thread_name = "UNKNOWN";

Semaphore::Semaphore(uint32_t count) {
    if (sem_init(&m_semaphore, 0, count)){
        FATAL("seminit() semaphore initializing failed.");
        throw std::system_error();
    }
}

Semaphore::~Semaphore() {
    sem_destroy(&m_semaphore);
}

void Semaphore::wait() {
    if (sem_wait(&m_semaphore)) {
        FATAL("sem_wait() error.");
        throw std::system_error();
    }
}

void Semaphore::notify() {
    if (sem_post(&m_semaphore)) {
        FATAL("sem_post() error.");
        throw std::system_error();
    }
}

// 封装线程执行需要的数据结构，作为 pthread_create 中Thread::Run() 的参数 
struct ThreadData {
    using ThreadFunc = Thread::ThreadFunc;
    ThreadFunc m_callback;
    std::string m_name;
    pid_t* m_id;
    Semaphore* m_semaphore;

    ThreadData( ThreadFunc func, 
                const std::string& name, 
                pid_t* tid,
                Semaphore* sem):
            m_callback(func),
            m_name(name),
            m_id(tid),
            m_semaphore(sem) {}
    
    void runInThread() {
        *m_id = Log::GetThreadId(); // ::syscall(SYS_gettid)
        m_id = nullptr; // ？？我擦，我不理解
        m_semaphore -> notify(); // 信号量加一 通知主线程子线程启动成功 
        m_semaphore = nullptr; // ？？我擦，我不理解
        t_tid = Log::GetThreadId();
        t_thread_name = m_name.empty() ? "UNKNOWN" : m_name;
        try {
            m_callback();
        } catch (const std::exception& e) {
            FATAL("Thread execute failed, name = %s, reason = %s.", 
                    t_thread_name.c_str(), e.what());
        }
    }
};



pid_t Thread::GetThisId() { return t_tid; }

const std::string& Thread::GetThisThreadName() {return t_thread_name; }

void Thread::SetThisThreadName(const std::string& name) {
    t_thread_name = name;
}

Thread::Thread(ThreadFunc callback, const std::string& name): 
    m_id(-1), 
    m_name(name), 
    m_thread(0),
    m_callback(callback), 
    m_semaphore(0),
    m_started(true),
    m_joined(false) {
    ThreadData* data = new ThreadData(m_callback, m_name, &m_id, &m_semaphore);
    int result = pthread_create(&m_thread, nullptr, &Thread::Run, data);
    if (result) {
        m_started = false;
        delete data;
        FATAL("pthread_create() failed, name = %s, errno = %d",
                m_name.c_str(), result);
        throw std::system_error();
    }
    else {
        m_semaphore.wait(); // 等待子线程启动     
        assert(m_id > 0); // m_id 储存 syscall() 获取的系统线程 id
    }
}

Thread::~Thread() {
    // 将子线程剥离主线程
    if (m_started && !m_joined) pthread_detach(m_thread);
}

int Thread::join() {
    assert(m_started);
    assert(!m_joined);
    m_joined = true;
    return pthread_join(m_thread, nullptr);
}

void* Thread::Run(void* arg) {
    std::unique_ptr<ThreadData> data((ThreadData*)arg );
    data -> runInThread();
    return 0;
}

