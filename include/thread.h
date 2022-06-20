// 封装线程模块，将信号量、锁等封装进去，方便使用

#ifndef __THREAD_H__
#define __THREAD_H__

#include "mutex.h"
#include <sys/types.h>    // pid_t 等类型
#include <functional>   // std::function 封装器
#include <string>

namespace liux {

class Thread: public Nocopyable {

public:
    using ptr = std::shared_ptr<Thread>;

    Thread(std::function<void()> cb, const std::string& name);

    ~Thread();

    pid_t getId() const {return m_id; }
    const std::string& getName() const {return m_name; }

    // 等待线程执行完毕   
    void join();
    // 获取当前线程指针。为什么要静态？不理解啊。
    static Thread* GetThis();
    // 获取当前线程名。为什么要静态？不理解啊。      
    static const std::string& GetName();
    // 设置当前线程名。为什么要静态？不理解。      
    static void SetName(const std::string& name);

private:
    // 线程执行函数，为什么要静态？
    static void* run(void* arg);

private:
    pid_t m_id = -1;            // pid
    pthread_t m_thread = 0;     // 线程结构 from <pthread.h>
    // 封装空参空返回的 线程执行 函数，需要参数则 bind (位于 functional) 绑定
    std::function<void()> m_cb;
    std::string m_name;         // 线程名
    Semaphore m_semaphore;      // 信号量
};

} // end namespace liux

#endif // __THREAD_H__