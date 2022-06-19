// 线程的封装实现     

#include "thread.h"
#include "log.h"
#include "util.h"
#include <exception> // throw 等异常相关

namespace liux {
// 如果是静态的，那我一个类只能维护一个线程，这合理吗？
// 如果是单例，也没有进行单例封装啊，而且线程构造函数为什么要单例？
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
        rt << " name=" name;
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
        
    }
}



} // end namespace liux