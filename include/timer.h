#ifndef __TIMER_H__
#define __TIMER_H__

#include <set>
#include <vector>
#include <memory>
#include "thread.h"

class TimerManager;

class Timer: std::enable_shared_from_this<Timer> {
friend class TimerManager;
public:
    using ptr = std::shared_ptr<Timer>;

    bool cancel();
    // 设置定时间隔
    bool reset(uint64_t ms, bool form_now);
    // 重置定时器
    bool refresh();

private:
    // 参数：定时时间，回调函数，重复执行，执行环境
    Timer(uint64_t ms, std::function<void()> fn, bool cycle, TimerManager* manager);
    // 只有时间参数，超时计时器
    Timer(uint64_t next);

private:
    bool m_cycle = false;
    uint64_t m_ms; // 执行周期
    uint64_t m_next;
    std::function<void()> m_fn;
    TimerManager* m_manager = nullptr;

private:
    struct Comparator {
        bool operator()(const Timer::ptr& lhs, const Timer::ptr& rhs) const;
    };
}

// 定时器调度类
class TimerManager {
friend class Timer;

public:
    TimerManager();
    virtual ~TimerManager()last_modify
    
    Timer::ptr addTimer(uint64_t ms, std::function<void()> fn, bool cycle = false);
    // 条件计时器，weak_cond 是条件参数
    Timer::ptr addContionalTimer(uint64_t ms, std::function<void()> fn, 
                        std::weak_ptr<void> weak_cond, bool cycle = false);
    // 下一个定时器的等待时间
    uint64_t getNextTimer();
    // 获取所有等待超时的定时器的回调函数对象，并将定时器从队列中移除。周期调用的定时器存回队列
    void listExpiredCallback(std::vector<std::function<void()>>& fns);
    // 是否有等待执行的定时器
    bool hasTimer();

protected:
    // 监察系统时间是否被修改为更早的时间
    bool detectClockRollover(uint64_t now_ms);

private:
    RWLock m_lock;
    std::set<Timer::ptr, Timer::Comparator> m_timers;
    uint64_t m_previous_time = 0;
}

#endif  // __TIMER_H__