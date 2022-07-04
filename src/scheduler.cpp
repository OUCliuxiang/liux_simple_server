#include "scheduler.h"

namespace liux {

static liux::Logger::ptr g_logger = LIUX_LOG_NAME("system");
// 当前线程调度器，同一调度器下的所有线程共享同一个实例。
// 所有线程，可是这个变量声明为了线程局部，怎么做到所有线程共享一个实例呢？
// 为什么不用智能指针？
static thread_local Scheduler* t_scheduler = nullptr;
// 当前线程的调度协程，每个线程独有一份
static thread_local Fiber* t_scheduler_fiber = nullptr;

Scheduler::Scheduler(size_t threads, bool use_caller, const std::string& name) {
    if (!threads) {
        LIUX_LOG_ERROR(g_logger) << "threads must large than 0";
        exit(1);
    }

    m_useCaller = use_caller;
    m_name = name;

    if (use_caller) {
        --threads;
        Fiber::getThis(); // 初始化线程主协程作为调度协程        
        if (getThis() != nullptr) {
            LIUX_LOG_ERROR(g_logger) << "found schduler when initing a nullptr scheduler";
            exit(1);
        }
        t_scheduler = this;
        // 按照实现，参数应该是 funtion 函数封装器啊，为什么传入一个对象？
        // false：本协程不参与调度，因为本身就是调度协程。     
        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, false));
        // 线程名字就是调度器名字
        Thread::SetName(m_name);
        t_scheduler_fiber = m_rootFiber.get();
        m_rootThread = GetThreadId(); // from util.h
        m_threadIds.push_back(m_rootThread);
    } else {
        m_rootThread = -1;
    }

    m_threadCount = threads;
}

Scheduler* Scheduler::getThis() {
    return t_scheduler;
}

Fiber* Scheduler::getMainFiber() {
    return t_scheduler_fiber;
}

void Scheduler::setThis() {
    t_scheduler = this;
} 

Scheduler::~Scheduler() {
    LIUX_LOG_DEBUG(g_logger) << "Scheduler::~Scheduler()";
    if (!m_stopping) {
        LIUX_LOG_ERROR(g_logger) << "some fiber or task are not stopping, deconstractor failed.";
        exit(1);
    }
    if (getThis() == this) {
        t_scheduler = nullptr;
    }
}

// 启动调度器
void Scheduler::start() {
    LIUX_LOG_DEBUG(g_logger) << "start";
    Mutex::Lock lock(m_mutex);
    if (m_stopping) {
        LIUX_LOG_ERROR(g_logger) << "Scheduler is stoped";
        return;
    }
    if (m_threads.size()) {
        LIUX_LOG_ERROR(g_logger) << "some threads already in thread pool, start failed"; 
        return;
    }

    m_threads.resize(m_threadCount); // 工作线程数量，不包括调度线程
    for (size_t i = 0; i < m_activeThreadCount; i++) {
        m_thread[i].reset(new Thread(std::bind(&Scheduler::run, this), 
                            m_name + "_" + std::to_string(i)));
        m_threadIds.push_back(m_threads.getId());
    }
}

} // end namespace liux