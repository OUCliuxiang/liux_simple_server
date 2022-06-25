// 协程模块实现

#include <atomic>
#include "fiber.h"
#include "log.h"
#include "config.h"

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
// 首先，该变量是静态的，属于这个类而不是类的具体对象，这是显然的，
// 因为这个变量应该属于线程，不可能每个协程都持有一个。
// 当然这里比较疑惑的是，那我直接把这个变量交付给线程，协程为什么还要维护它？
// 其次，该变量是线程局部的，也即每个线程持有一个该变量，这也是显然的，
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
}
} // end namespace liux 
