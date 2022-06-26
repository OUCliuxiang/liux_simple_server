// 协程模块实现

#include <atomic>
#include "fiber.h"
#include "log.h"
#include "config.h"
#include <assert.h>

namespace liux {

// 全局静态： g_logger == system logger
static Logger::ptr g_logger = LIUX_LOG_NAME("system");

// 全局静态： 原子类封装，初始化为 0，用于生成协程 ID     
// std::atomic<T> 封装一个 T 类型的原子变量，由于 s_fiber_id 是 
// 定义在函数外的全局静态变量，无法用锁，则使用 CAS 机制保证原子性的
// std::atomic<T> 保证其在多线程编程中的线程安全。
static std::atomic<uint64_t> s_fiber_id{0};
// 统计当前协程数
static std::atomic<uint64_t> s_fiber_count{0};

// 线程局部变量，当前线程正在运行的协程。
// -------------------------------
// 这个静态线程局部变量声明的理解：
// 首先，该变量是静态的，因为它属于这个命名空间而不是任何一个类。
// 其次，该变量是线程局部的，也即每个线程持有一个该变量副本，这也是显然的，
// 因为是线程正在运行的协程，显然应该是每个线程持有一个。
static thread_local Fiber* t_fiber = nullptr;
// 当前线程的主协程，切换到这个协程就相当于在来到了线程主函数
static thread_local Fiber::ptr t_thread_fiber;

// 协程栈大小，配置变量类智能指针形式，可以从配置文件读取，缺省是 128k
static ConfigVar::ptr g_fiber_stack_size = 
    Config::Lookup("fiber.stack_size", 128*1024, "fiber stack size");

//  封装 malloc 为协程栈内存分配器。该类归协程类所有。 
class StackAllocator {
public:
    static void *Alloc(size_t size) {return malloc(size);}
    static void Dealloc(void* vp, size_t size) {return free(vp);}
};

// 获取正在运行的协程的 id  
uint64_t Fiber::getFiberId() {
    if (t_fiber) {
        return t_fiber -> getId();
    }
    return 0;
}

// private 空参构造，用于实例化线程的第一个协程，只允许 getThis() 调用          
Fiber::Fiber() {
    m_state = RUNNING;

    // 保存当前协程寄存器中的信息到 m_ctx，成功返回 0。    
    // 当前为构造阶段，寄存器无信息，相当于初始化。    
    if (getcontext(&m_ctx)) {
        LIUX_LOG_ERROR(g_logger) << "getcontext error";
        assert(false); // 直接断言失败，退出
    }

    // 一下两个都是全局静态原子性的，也即不论线程而统计全部协程
    ++ s_fiber_count;
    m_id = s_fiber_id ++; // 协程 id 从 0 开始渐增
    LIUX_LOG_INFO(g_logger) << "Fiber::Fiber() main id = " << m_id;
    
    setThis(this);  // 设置正在运行的协程为当前协程
}

// 设置正在运行的协程为 f          
void Fiber::setThis(Fiber* f) {
    // 再次提醒 t_fiber 是线程局部的
    t_fiber = f; 
}

// 获取线程当前正在运行的协程，同时充当初始化线程主协程的作用。在使用协程前要先调用一下。
Fiber::ptr Fiber::getThis() {
    // 继承了 enable_shared_ptr_from_this 的类拥有以下获取自身智能指针的方法。
    if (t_fiber) return t_fiber -> shared_from_this();

    // 执行 private 的无参构造
    Fiber::ptr main_fiber(new Fiber());
    // std::shared:ptr::get() 返回智能指针所封装的裸指针
    if (t_fiber != main_fiber.get())
        LIUX_LOG_ERROR(g_logger) << "new Fiber() or Fiber::setThis() error";
    assert(t_fiber == main_fiber.get());

    t_thread_fiber = main_fiber;
    // t_thread -> shared_from_this() 和 t_thread_fiber 是否相同？
    // 按照前面语句，main_fiber 包装的指针就是 t_fiber 。
    return t_fiber -> shared_from_this();
}

} // end namespace liux 
