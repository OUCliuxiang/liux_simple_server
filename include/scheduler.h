// 协程调度模块

#ifndef __SCHERULER_H__
#define __SCHEDULER_H__

#include <functional>
#include <memory>
#include <sys/types>
// #include <stdint>

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
    // use_caller 是否将当前线程池也作为调度线程
    // name 名字
    Scheduler(size_t threads=1, bool use_caller=true, const std::string& name);

private:
    size_t m_threads;
    bool m_use_caller;
    std::string m_name;

};

} // end namespace liux


#endif // __SCHEDULER_H__