// 协程模块实现

#include <atomic>
#include "fiber.h"
#include "log.h"
#include "config.h"
// #include <cassert>  // 妈的老是报错，不用了

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

// 全局静态线程局部变量，当前线程正在运行的协程。
// -------------------------------
// 这个全局静态 线程局部变量声明的理解：
// 首先，该变量是静态的，因为它属于这个文件，作用域不能超出该文件。
// 其次，该变量是线程局部的，也即每个线程持有一个该变量副本，这也是显然的，
// 因为是线程正在运行的协程，显然应该是每个线程持有一个。
static thread_local Fiber* t_fiber = nullptr;
// 当前线程的主协程，切换到这个协程就相当于在来到了线程主函数
static thread_local Fiber::ptr t_thread_fiber;

// 协程栈大小，配置变量类智能指针形式，可以从配置文件读取，缺省是 128k
// 如果含参构造中没有指明 stacksize，就用这个缺省。
static ConfigVar<uint32_t>::ptr g_fiber_stack_size = 
    Config::Lookup<uint32_t>("fiber.stack_size", 128*1024, "fiber stack size");

// 封装 malloc 为协程栈内存分配器。该类归协程类所有。 
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
// 没有实际需要运行的函数，故不分配栈
Fiber::Fiber() {
    m_state = RUNNING;

    // 保存当前协程寄存器中的信息到 m_ctx，成功返回 0。    
    // 当前为构造阶段，寄存器无信息，相当于初始化。    
    if (getcontext(&m_ctx)) {
        LIUX_LOG_ERROR(g_logger) << "getcontext error";
        // std::assert(false); // 直接断言失败，退出
        exit(1);
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
    if (t_fiber != main_fiber.get()){
        LIUX_LOG_ERROR(g_logger) << "new Fiber() or Fiber::setThis() error";
        // std::assert(t_fiber == main_fiber.get());
        exit(1);
    }
    t_thread_fiber = main_fiber;
    // t_thread -> shared_from_this() 和 t_thread_fiber 是否相同？
    // 按照前面语句，main_fiber 包装的指针就是 t_fiber 。
    return t_fiber -> shared_from_this();
}

// 带参构造，用于实际运行而非只营分配之用，需要分配栈
// 头文件谢了缺省参数值，此处就不用给了，给的话编译器会报 warning
Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool run_in_scheduler): 
    m_id(s_fiber_id++), m_cb(cb), m_runInScheduler(run_in_scheduler) {
    ++ s_fiber_count; // 新建协程，协程总量 ++
    m_stacksize = stacksize ? stacksize : g_fiber_stack_size -> getValue();
    m_stack     = StackAllocator::Alloc(m_stacksize);

    if (getcontext(&m_ctx)) {
        LIUX_LOG_ERROR(g_logger) << "getcontext error" ;
        exit(1);
    }

    m_ctx.uc_link = nullptr;    // 当前上下文结束时要恢复到的上下文，其中上下文由 makecontext() 创建
    m_ctx.uc_stack.ss_sp = m_stack; // 当前上下文所使用的栈
    m_ctx.uc_stack.ss_size = m_stacksize; // 栈大小

    // 上下文切换到  Fiber::mainFunc 上面，上下文保存到 m_ctx
    // 第二个参数传入的是函数入口地址
    // 函数返回时如果 m_ctx.uc_link 为空，则退出执行，否则转移到 uc_link 指向的上下文 
    makecontext(&m_ctx, &Fiber::mainFunc, 0); // 那 cb 用来干嘛？     

    LIUX_LOG_INFO(g_logger) << "Fiber::Fiber() id = " << m_id;
}

// 协程析构时的处理有没有分配栈要分开考虑   
Fiber::~Fiber() {
    LIUX_LOG_INFO(g_logger) << "Fiber::~Fiber() id = " << m_id;
    --s_fiber_count;
    if (m_stack) { // 有栈，是子协程，确保子协程一定要结束
        if (m_state != TERM) {
            LIUX_LOG_ERROR(g_logger) << "non-main fiber deconstracted in non-term state";
            exit(1);
        }
        StackAllocator::Dealloc(m_stack, m_stacksize);
        LIUX_LOG_INFO(g_logger) << "dealloc stack, id = " << m_id; 
    } else {    // 没有栈，说明是主协程
        if (m_cb ) { // 主协程必没有入口函数
            LIUX_LOG_INFO(g_logger) << "m_cb is found but it is main fiber, place check the code logic";
            exit(1);
        }
        if (m_state != RUNNING) {// 需要析构时其他协程必已经退出，主协程必然处于 RUNNING 状态 
            LIUX_LOG_INFO(g_logger) << "main fiber is not in running state";
            exit(1); 
        }
        Fiber *cur = t_fiber; // 线程当前正在运行的协程必是 this
        if (cur == this) {
            setThis(nullptr);
        } 
    }
}

// // 重置协程状态和入口函数，复用栈，不需要重新创建栈。规定只有处于 TERM 状态的协程可以 reset 
void Fiber::reset(std::function<void()> cb) {
    if (!m_stack) { // 主协程无法重置，其只负责调度
        LIUX_LOG_ERROR(g_logger) << "the main fiber cannot be reset";
        exit(1);
    }
    if (m_state != TERM) {
        LIUX_LOG_ERROR(g_logger) << "only fiber in TERM state can be reset";
        exit(1);
    }
    m_cb = cb;
    if (getcontext(&m_ctx)) {
        LIUX_LOG_ERROR(g_logger) << "getcontext error";
        exit(1);
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::mainFunc, 0);
    m_state = READY;
}

// 处于 READY 态的协程和当前正在运行协程交换，该协程进入 RUNNING，前者变为 READY
void Fiber::resume() {
    if (m_state != READY) {
        LIUX_LOG_ERROR(g_logger) << "only fiber in READY state can be resumed";
        exit(1);
    }
    setThis(this);
    m_state = READY;
    // 如果协程参与调度，那么应该和调度器主协程进行 swap，而不是线程主协程
    if (m_runInScheduler) { // 调度器暂未实现，后面补上
        
    } else { // 否则
        if (swapcontext(&(t_thread_fiber -> m_ctx), &m_ctx)){
            LIUX_LOG_ERROR(g_logger) << "swapcontext error";
            exit(1);
        }
    }
}

// 协程运行完会自动 yield 一次，用于回到主协程。如果参与调度器，回到调度器主协程
// 调度器还没有实现，次出现空着，实现了调度器回头再补上
void Fiber::yield() {
    
}

void Fiber::mainFunc() {
    // 这是个静态的，用来接收当前运行协程的 function<void()> cb函数入口
    // getThis 此时肯定非空，其中的 shared_from_this() 调用会使当前 fiber shared_ptr 的引用计数+1 
    Fiber::ptr cur = getThis();
    if (!cur) {
        LIUX_LOG_ERROR(g_logger) << "receive no fiber" ;
        exit(1);
    }

    // 静态函数里面可以直接修改 private 变量？
    cur -> m_cb(); // 执行
    cur -> m_cb = nullptr; // 执行结束，我操，怎么直接置 null 了？
    cur -> m_state = TERM;

    auto raw_ptr = cur.get(); // 获取裸指针，相应地 ptr 引用计数减一
    cur.reset(); // 使用 . 操作符？ 不传入参数？这显然不能执行啊
    raw_ptr -> yield();


}

} // end namespace liux 
