// 协程模块

#ifndef __FIBER_H__
#define __FIBER_H__

#include <memory>
#include <ucontext> // 保存协程上下文（状态）
#include <functional>
#include "thread.h" // 线程调用协程，协程可以理解为可中断的函数

namespace liux {

// std::enable_shared_ptr_from_this<T> from <memory> ，继承该类的类
// 拥有成员函数 std::shared_from_this() 返回一个指向自身的智能指针，
// 用于类内成员需要传递当前类对象自身指针到其他函数的情况。使用裸指针 this 
// 不会加入到 shared_ptr 计数器，会造成计数混乱产生析构相关问题。
class Fiber: public std::enable_shared_ptr_from_this<Fiber> {
public:
    using ptr = std::shared_ptr<Fiber>;
    // 自定义协程状态：就绪态，运行态，结束态
    enum State {
        READY,      // 就绪态，刚刚建立或者 yield 之后的状态
        RUNNING,    // 运行态，resume 之后的状态
        TERM        // 结束态，协程回调函数执行完之后的状态
    };

private:
    // 无参构造只能用于构造线程的第一个协程，也即线程主函数对应的协程
    // 这个协程只能由 Fiber::getThis() 方法调用，将其私有。但不是单例
    Fiber();

public:
    // 有参构造，用于创建用户协程
    // cb               协程入口函数，使用 function 封装代替函数指针，此处空参空返回，
    //                  需要参数可以通过 bind 绑定
    // stacksize        协程栈大小
    // run_in_scheduler 本协程是否参与协程调度器调度，默认 true
    Fiber(std::function<void()> cb, size_t stacksize=0, bool run_in_scheduler=true);
    // 缺省无参析构可以完成对象堆资源回收，此处显示声明了无参析构，一定要实现。
    ~Fiber();

    // 重置协程状态和入口函数，复用栈，不需要重新创建栈
    void reset(std::function<void()> cb);

    // 和正在运行的协程交换，当前协程切换到执行状态 RUNNING，前者变为 READY，
    void resume();

    // 和上次 resume 退出到后台的协程交换，当前协程让出执行权为 READY 状态，前者 RUNNING
    void yield();

    // 获取协程 id
    uint64_t getId() const {return m_id;}

    // 获取协程状态
    State getState() const {return m_state;}

    // 设置当前正在运行的协程，即设置线程的局部变量 t_fiber 的值
    static void setThis(Fiber *f);

    // 静态函数，需要指明 ptr 所属的定义域
    // 返回当前线程正在执行的协程，如果当前线程还未创见协程，则创建线程第一个协程作为线程主协程
    // 线程主协程负责调度其他协程，其他协程结束时要返回到主协程，由主协程选择新的协程 resume 
    // 线程如果要创建协程，应首先执行该函数以初始化主协程。
    static Fiber::ptr getThis();

    // 获取总的协程数
    static uint64_t totalFibers();

    // 携程入口函数？怎么又一个？先 mark 住，根据 fiber.cpp 再行理解吧。
    static void mainFunc();

    // 获取当前正在执行的协程的 id，有点理解，又有些费解，根据 fiber.cpp 再看吧
    static uint64_t getFiberId();

private:
    uint64_t m_id = 0;
    uint64_t m_stacksize = 0;
    State m_state = READY;
    ucontext_t m_ctx;   // 协程上下文
    void *m_stack = nullptr;    // 协程栈入口
    std::function<void()> cb;   // 协程函数入口
    bool m_runInScheduler;      // 是否参与协程调度器调度

};

} // end namespace liux

#endif // __FIBER_H__