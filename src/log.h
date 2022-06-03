#ifndef __SYLAR_LOG_H__ 
#define __SYLAR_LOG_H__

#include <string>
#include <stdint.h> 
#include <memory>

namespace sylar {   // 为了避免和其他人的代码有方法或变量的相同名字         

// 日志事件，记录日志现场    
class LogEvent {
public:
    // 智能指针管理对象。       
    using ptr = std::shared_ptr<LogEvent>;
    LogEvent();

private:
    const char* m_file = nullptr;   // 文件名
    uint32_t m_line = 0;        // 行号
    uint64_t m_elapse = 0       // 程序启动到现在的毫秒数    
    uint32_t m_threadId = 0;    // 线程号
    uint32_t m_fiberId = 0;     // 协程号 
    uint64_t  m_time = 0;       // 时间戳，到毫秒
    std::string m_content;      // 日志内容
};

// 日志级别器     
class LogLevel{
public:
    // 枚举 日志级别    
    enum Level {
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };
};

// 日志输出目的地      
class LogAppender{
public:
    using ptr = std::shared_ptr<LogAppender>;
    // 日志输出目的地可能有很多，这里要定义成虚类，其自身无法实现，只能让子类去继承
    virtual ~LogAppender() {}
    void log(LogLevel::Level level, LogEvent::ptr event);

private:
    LogLevel::Level m_level;
};

// 日志输出器
class Logger{
public:
    using ptr = std::shared_ptr<Logger>;
    Logger(const std::string& name = "root");
    void log(LogLevel::Level level, LogEvent::ptr event);

private:
    std::string m_name;
    LogLevel::Level m_level;
    LogAppender::ptr;
};

// 日志格式化器
class LogFormatter{
public:
    using ptr = std::shared_ptr<LogFormatter>;
    std::string format(LogEvent::ptr event);
private:

};

// 输出到控制台  
class StdoutLogAppender: LogAppender{

}

// 输出到文件      
class FileLogAppender: LogAppender{

}

}

#endif