#ifndef __LOG_H__
#define __LOG_H__

#include "util.h"

#include <stdio.h>  // vprintf 可变参数系列函数
#include <sys/time.h>   //time_t 时间相关
#include <string>   
#include <cstdarg>
#include <memory>
#include <sstream>
#include <fstream>
#include <vector>
#include <list>
#include <map>


// 定义系列宏         

// 获取 root 日志器
#define LIUX_LOG_ROOT liux::LoggerMgr::GetInstance() -> getRoot()

// 获取指定名称日志器 
#define LIUX_LOG_NAME(name) liux::LoggerMgr::GetInstance() -> getLogger(name)

// __FILE__ 返回当前文件路径         
// __LINE__ 返回当前行     
#define LIUX_LOG_LEVEL(logger, level) \
    if (level >= logger -> getLevel()) \
        liux::LogEvent::ptr(new liux::LogEvent(logger -> getName(), level, __FILE__, \
        __LINE__, liux::GetElapse(), liux::GetThreadId(), liux::GetFiberId(), \
        time(NULL), liux::GetThreadName())) -> getContent()

#define LIUX_LOG_INFO(logger) LIUX_LOG_LEVEL(logger, liux::LogLevel::INFO)
#define LIUX_LOG_DEBUG(logger) LIUX_LOG_LEVEL(logger, liux::LogLevel::DEBUG)
#define LIUX_LOG_WARN(logger) LIUX_LOG_LEVEL(logger, liux::LogLevel::WARN)
#define LIUX_LOG_ERROR(logger) LIUX_LOG_LEVEL(logger, liux::LogLevel::ERROR)
#define LIUX_LOG_FATAL(logger) LIUX_LOG_LEVEL(logger, liux::LogLevel::FATAL) 

// __VA_AGRS__ 宏接收的可变参数列表            
#define LIUX_LOG_FMT_LEVEL(logger, level) \
    if (level >= logger -> getLevel()) \


// 宏定义结束

namespace liux{

class LogLevel{
public:
    enum Level{
        UNKNOWN = 0, 
        INFO = 1,
        DEBUG = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5,
    };

    static const char* toString(Level);
    static Level toLevel(const std::string&);
};


class LogEvent{
public:
    using ptr = std::shared_ptr<LogEvent>; // 智能指针来自于 <memory> 头文件

    LogEvent(const std::string& loggerName, 
             LogLevel::Level level, 
             const char* file, 
             uint32_t line, 
             uint64_t elapse, 
             uint32_t threadId, 
             uint32_t fiberId, 
             time_t time, 
             const std::string& threadName): 
             
             m_loggerName(loggerName), 
             m_level(level), 
             m_file(file), 
             m_line(line), 
             m_elapse(elapse), 
             m_threadId(threadId), 
             m_fiberId(fiberId), 
             m_time(time), 
             m_threadName(threadName) {}
    
    const LogLevel::Level getLevel() const {return m_level;}
    const std::string getContent() const {return m_ss.str();}
    std::stringstream& getSS() {return m_ss;}
    const char* getFile() const {return m_file;}
    uint32_t getLine() const {return m_line;}
    uint64_t getElapse() const {return m_elapse;}
    uint32_t getThreadId() const {return m_threadId;}
    uint32_t getFiberId() const {return m_fiberId;}
    time_t getTime() const {return m_time;}
    const std::string getThreadName() const {return m_threadName;}
    const std::string getLoggerName() const {return m_loggerName;}
    void setLoggerName(const std::string str) {m_loggerName = str;}
    // 可变参数打印到标准输出
    void printf(const char* fmt, ...);

private:
    std::string m_loggerName;       // 日志器名
    LogLevel::Level m_level;
    std::stringstream m_ss; // 存储日志内容到字符串流，方便流式写文件。头文件 <sstream>
    const char* m_file      = nullptr;
    uint32_t    m_line      = 0;
    uint64_t    m_elapse    = 0;    // 日志器创建到现在的时间毫秒
    uint32_t    m_threadId  = 0;
    uint32_t    m_fiberId   = 0;    // 协程 ID
    time_t      m_time;             // UTC 时间戳
    std::string m_threadName;       // 线程名字
};


/**
    日志格式化器，具体格式项有：      
    %m  消息体 
    %p  日志级别
    %c  日志器名字
    %d  日期时间，后面可以大括号里跟具体格式模板如 %d{%Y-%m-%d %H:%M:%S}
    %r  日志器创建到现在的时间毫秒
    %l  行号
    %t  线程ID
    %f  文件名
    %F  协程ID
    %N  线程名
    %T  制表符
    %n  换行符
    %%  百分号
*/
class LogFormatter{
public:
    using ptr = std::shared_ptr<LogFormatter>;

    // 默认格式模板： 年-月-日 时:分:秒 [累计运行毫秒] 线程ID 线程名 协程ID [日志级别] [日志器名称]: 文件名 行号 消息体
    // 这里先声明，不要实现。实现需要调用 init 函数，为保证不出错，在外部调用吧。        
    LogFormatter(const std::string& pattern = 
        "%d{%Y-%m-%d %H:%M:%S} [%rms]%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n");
    
    // 初始化日志器，解析模板 m_pattern，提取各具体格式项并实例化 FormatItem 的各派生类对象
    void init();    
    std::string getPattern() const {return m_pattern;}
    bool isError() const {return m_error;}

    // 格式化日志事件，返回格式化后的字符串文本
    std::string format(LogEvent::ptr event);
    // 格式化日志事件，返回格式化后的标准输出流
    void format(std::ostream& os, LogEvent::ptr event);

    // 只能在 LogFormatter 实例化的 item 类，抽象类，必须且只能派生出解析不同类型格式化项的子类
    class FormatItem {
    public:
        using ptr = std::shared_ptr<FormatItem>;
        // 析构一定要需，否则基类指针指向子类对象时子类析构不会调用。
        virtual ~FormatItem() {}     
        // 纯虚函数，不可实现。子类必须根据特定的格式化项实现自己的 format 解析方法。
        // 纯虚函数，该类是抽象类，无法实例化。  
        // 传入的本来就是指针了，不需要 const 引用。
        virtual void format(std::ostream& os, LogEvent::ptr event) = 0;
    };

private:
    // 日志格式模板，按照这个格式化模板来输出日志   
    // 比如 "%p %f %d{%Y:%M:%D %H:%M:%S}: %l: %c"
    std::string m_pattern;  

    // 显然就像上面这个模板，有好多格式化项，每个项都要有一个特定的具体的不同的解析对象
    // 解析项由基类 FormatItem 派生出来，可以有基类指针指向，所有具体解析对象指针放置在数组里
    std::vector<FormatItem::ptr> m_items;
    bool m_error = false;
};


// 日志输出目的地，把日志输出到哪里？
class LogAppender {
public:
    using ptr = std::shared_ptr<LogAppender>;

    LogAppender(LogFormatter::ptr formatter);
    // 输出地必须是具体的文件或者标准输出，这是一个抽象类作为基类，析构要虚
    virtual ~LogAppender() {} 

    void setFormatter(LogFormatter::ptr formatter);
    LogFormatter::ptr getFormatter();

    // 纯虚，依靠子类去实现。向目的地写入日志。
    virtual void log(LogEvent::ptr event) = 0;

protected:
    // 拥有一个可设置格式化器和一个默认的格式化器，日志事件格式化后再输出
    LogFormatter::ptr m_default_formatter;
    LogFormatter::ptr m_formatter;
};


// 日志写到标准输出（控制台）
class StdoutLogAppender: public LogAppender {
public:
    using ptr = std::shared_ptr<StdoutLogAppender>;
    StdoutLogAppender();

    void log(LogEvent::ptr event) override;
};


// 日志写到文件
class FileLogAppender: public LogAppender {
public:
    using ptr = std::shared_ptr<FileLogAppender>;

    FileLogAppender(const std::string& filename);
    
    void log(LogEvent::ptr event) override;
    // 重新打开输出流
    bool reopen(); 

private:
    std::string m_filename;
    // 文件输出流，流式写文件。
    std::ofstream m_filestream;
    bool m_reopenError = false; // 打开失败标志位
    uint64_t m_lastTime; // 记录上一次打开输出流的时间，如果超过某阈值，log 时要重新打开
};


// 日志器
class Logger {
public:
    using ptr = std::shared_ptr<Logger>;
    Logger(const std::string& name = "default");

    const std::string& getName() const {return m_name; }
    const uint64_t getCreateTime() const {return m_createTime; }
    const LogLevel::Level getLevel() const {return m_level; }
    
    void setLevel(LogLevel::Level level) {m_level = level;}
    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    void clearAppender();
    void log(LogEvent::ptr event);

private:
    std::string m_name;
    // 日志器等级，用于筛选可输出的日志事件
    LogLevel::Level m_level = LogLevel::INFO;
    // LogAppender 集合，一个日志器可能有很多输出目的地
    std::list<LogAppender::ptr> m_appenders;
    // 日志器创建时间    
    uint64_t m_createTime = 0;
};


// 日志事件包装器，包含日志器和事件，方便宏定义
class LogEventWarp {
public:
    using ptr = std::shared_ptr<LogEventWarp>;
    LogEventWarp(Logger::ptr logger, LogEvent::ptr event);
    // 析构实现为：包装器析构的时候写日志
    ~LogEventWarp();
    LogEvent::ptr getLogEvent() const {return m_event;}

private:
    Logger::ptr m_logger;
    LogEvent::ptr m_event;
};


// 日志器管理模块 
class LoggerManager {
public:
    using ptr = std::shared_ptr<LoggerManager>;
    LoggerManager();
    void init();
    Logger::ptr getLogger(const std::string& name);
    Logger::ptr getRoot() const {return m_root;}

private:
    std::map<std::string, Logger::ptr> m_loggers;
    Logger::ptr m_root; // 根日志器
};

// 单例，裸指针
using LoggerMgr = liux::Singleton<LoggerManager>;

} // end namespace liux

#endif // __LOG_H__