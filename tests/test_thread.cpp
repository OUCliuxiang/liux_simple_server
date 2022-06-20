#include "thread.h"
#include "log.h"
#include <vector>
#include <iostream>

liux::Logger::ptr g_logger = LIUX_LOG_ROOT();

int count = 0;
liux::Mutex s_mutex;

void func1(void* arg) {
    LIUX_LOG_INFO(g_logger) << "name: " <<liux::Thread::GetName() 
        << " this.name: " << liux::Thread::GetThis() -> getName()
        << " this.id: " << liux::Thread::GetThis() -> getId() 
        << " thread name: " << liux::GetThreadName() 
        << " thread id: " << liux::GetThreadId() 
        << " arg: " << *(int*)arg;
    for (int i = 0; i < 10000; i ++) {
        liux::Mutex::Lock lock(s_mutex); // 互斥量加局部锁
        ++ count;
    } 
}

int main() {
    std::vector<liux::Thread::ptr> thrs;
    int arg = 123456;
    for (int i = 0; i < 3; i++) {
        liux::Thread::ptr thr(new liux::Thread(std::bind(func1, &arg), "thread_" + std::to_string(i)));
        thrs.push_back(thr);
    }

    for (int i = 0; i < 3; i++) {
        thrs[i] -> join();
    }

    LIUX_LOG_INFO(g_logger) << "count = " << count;
    return 0;
}