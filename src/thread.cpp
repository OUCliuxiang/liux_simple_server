// 线程的封装实现     

#include "thread.h"
#include "log.h"
#include "util.h"   //  liux::GetThreadId()
#include <exception> // throw 等异常相关

namespace liux {
// 全局静态线程局部变量
// 变量属于 liux 这个命名空间而不是任何一个类
// 变量每个线程独立维护一个，不与其他线程冲突      
static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKNOWN";

static Logger::ptr g_logger = LIUX_LOG_NAME("system");

Thread* Thread::GetThis() {return t_thread; }
const std::string& Thread::GetName() {return t_thread_name; }

void Thread::SetName(const std::string& name) {
    if (name.empty()) return; 
    if (t_thread)   t_thread -> m_name = name;
    t_thread_name = name; 
    // 什么操作，我不理解。
    // 类的静态成员指针指向的名字和静态变量代表的名字？
    // 那 本对象 m_name 线程名怎么设置？
}

Thread::Thread(std::function<void()> cb, const std::string& name):
    m_cb(cb), m_name(name) {
    if (name.empty())   m_name = "UNKNOWN";
    // m_thread 接收创建处的线程，run 是线程要执行的函数，this 是参数，为什么要传入 this ??
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if (rt) {
        LIUX_LOG_ERROR(g_logger) << "pthread_create thread fail, rt=" << 
        rt << " name=" << name;
        throw std::logic_error("pthread_create error");
    }
    m_semaphore.wait(); 
    // m_semaphore 默认初始化为 0，无资源，这里构造完直接等待？
}

Thread::~Thread() {
    if (m_thread) {
        pthread_detach(m_thread);
    }
}

void Thread::join() {
    if (m_thread) {
        // 阻塞的方式等待指定线程结束。函数返回时被等待线程资源收回。若已结束则函数立即返回。
        // 第二个参数 void** 空指针用来存储被等待线程的返回值。成功返回 0，失败返回错误号。    
        int rt = pthread_join(m_thread, nullptr);
        if (rt) {
            LIUX_LOG_ERROR(g_logger) << "pthread_join thread fail, rt=" <<
            rt << " name= " << m_name;
            throw std::logic_error("pthread_join error");
        }
    }
    // 线程结束 pid 置零 ？？那为什么不销毁该线程？
    m_thread = 0;
}

void *Thread::run(void *arg) {
    Thread *thread  = (Thread*) arg;
    t_thread        = thread; // 为什么维护一个静态变量来回相互赋值？
    t_thread_name   = thread -> m_name;
    thread -> m_id  = liux::GetThreadId(); // util.h, 获取本线程 ID ？ 
    pthread_setname_np(pthread_self(), thread -> m_name.substr(0, 15).c_str());
    std::function<void()> cb;
    cb.swap(thread -> m_cb);
    // 释放信号量，为何？
    thread -> m_semaphore.notify();
    cb();
    return 0;
}

} // end namespace liux