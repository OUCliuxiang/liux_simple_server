// 协程调度模块

#ifndef __SCHERULER_H__
#define __SCHEDULER_H__

#include <functional>
#include <memory>
#include <sys/types.h>
#include <vector>
#include <list>
#include <string>

#include "fiber.h"
#include "log.h"
#include "thread.h"
#include "mutex.h"

namespace liux {

// N 线程 M 协程 调度器，有一个线程池，协程在线程池中来回切换
class Scheduler {
public:
    using ptr = std::shared_ptr<Scheduler>;
    // threads 线程数
    // use_caller 是否将当前线程作为调度线程
    // name 名字
    Scheduler(size_t threads=1, bool use_caller=true, const std::string& name="Scheduler");
    // 虚析构，用于基类指针指向派生类对象，析构的时候释放子类资源
    virtual ~Scheduler();

    inline const std::string& getName() const {return m_name;}

    // 获取当前线程调度器
    static Scheduler* getThis();
    // 获取当前线程主协程
    static Fiber* getMainFiber();  // Fiber 不需要指定命名空间，当前在命名空间内部

    // 添加调度任务的模板函数。
    // T fc: T 类型的调度任务，可以是协程对象或者函数指针，thread 指定运行该任务的线程
    template <class T>
    void schedule(T fc, int thread = -1) {
        bool need_tickle = false; // 是否需要唤醒 idle 协程。 idle 携程是哪位？  
        {
            Mutex::Lock(m_mutex);
            need_tickle = scheduleNoLock(fc, thread);
        }
        if (need_tickle) tickle(); // 唤醒 idle 协程
    }

    // 启动调度器
    void start();

    // 停止调度器，等任务队列里面所有任务执行完毕再返回
    void stop();

protected:
    // 通知调度器有任务了，用来唤醒 idle 协程？
    virtual void tickle();

    // 协程调度函数
    void run();

    // 无任务调度时执行 idle 协程
    virtual void idle();

    // 返回是否可以停止 
    virtual bool stopping();

    // 设置当前调度器
    void setThis();

    // 返回是否有空闲线程
    // 调度协程进入 idle 时空闲线程数量加一，从 idle 协程返回时空闲线程数量减一。        
    inline bool hasIdleThreads()  {return m_idleThreadCount > 0;}

private:
    // 添加调度任务的模板函数，无锁
    template <class T>
    bool scheduleNoLock(T fc, int thread) {
        bool need_tickle = m_tasks.empty();
        ScheduleTask task(fc, thread);
        if (task.fiber || task.cb) m_tasks.push_back(task);
        return need_tickle;
    } 

    // 待调度的任务（协程或者函数）
    struct ScheduleTask {
        Fiber::ptr fiber;           // 接收协程
        std::function<void()> cb;   // 接收函数入口
        int thread;

        ScheduleTask(Fiber::ptr f, int thr) {
            fiber = f;
            thread = thr;
        }
        // 智能指针的指针使用了右值引用 swap 实现完美转发，有脱裤子放屁的嫌疑，此处略过不实现
        ScheduleTask(std::function<void()> f, int thr) {
            cb = f;
            thread = thr;
        }

        ScheduleTask() {
            thread = -1 ; // 无参构造，没有接到有效任务。后续判断 thread == -1 则报错退出
        }

        void reset() {
            fiber = nullptr;
            cb = nullptr;
            thread = -1;
        }
    };

private:
    std::string m_name;     // 协程调度器名称
    Mutex m_mutex;          // 互斥锁
    std::vector<Thread::ptr> m_threads;// 线程池
    std::list<ScheduleTask> m_tasks;    // 任务队列，任务可以是协程或者函数       
    std::vector<int> m_threadIds;       // 线程池中的线程 id 数组
    size_t m_threadCount = 0;   // 工作线程数量，不包括 use_caller 产生的主线程      
    std::atomic<size_t> m_activeThreadCount = {0};  // 活跃线程数，线程安全原子量
    std::atomic<size_t> m_idleThreadCount = {0};    // 空闲线程数，线程安全原子量

    bool m_useCaller;           // 是否将当前线程也作为调度线程
    Fiber::ptr m_rootFiber;     // 如果是，调度其所在县城的调度协程
    int m_rootThread = 0;       // 如果是，调度器所在的线程 id 
    bool m_stopping = false;    // 是否正在停止 

};

} // end namespace liux


#endif // __SCHEDULER_H__