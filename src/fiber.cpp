// 协程模块实现

#include <atomic>
#include "fiber.h"
#include "log.h"
#include "config.h"

namespace liux {

// 全局静态： g_logger == system logger
static Logger::ptr g_logger = LIUX_LOG_NAME("system");

// 全局静态： 用于生成协程 ID     
// std::atomic<T> 封装一个 T 类型的原子变量，由于 s_fiber_id 是 
// 定义在函数外的全局静态变量，无法用锁，则使用 CAS 机制保证原子性的
// std::atomic<T> 保证其在多线程编程中的线程安全。
static std::atomic<uint64_t> s_fiber_id{0};

} // end namespace liux 
