// 线程的封装实现     

#include "thread.h"
#include "log.h"
#include "util.h"

namespace liux {
// 如果是静态的，那我一个类只能维护一个线程，这合理吗？
static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKNOWN";

static Logger::ptr g_logger = LIUX_LOG_NAME("system");

Thread* Thread::GetThis() {return t_thread; }
const std::string& Thread::GetName() {return t_thread_name; }

} // end namespace liux