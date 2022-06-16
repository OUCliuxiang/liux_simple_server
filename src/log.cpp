#include <utility> // std::pair
#include <functional> // std::function
#include <iostream>
#include "log.h"

namespace liux{

const char* LogLevel::toString(LogLevel::Level level){
    switch (level) {
// 宏定义语句 define 中参数前面加 # 代表转为字符串  
#define XX(name) case LogLevel::name: return #name; 
    XX(INFO)
    XX(DEBUG)
    XX(WARN)
    XX(ERROR)
    XX(FATAL)
#undef XX
    default:    return "UNKNOWN";
    }
    return "UNKNOWN";
}

LogLevel::Level toLevel(std::string& val) {
#define XX(level, v) if (val == #v) return LogLevel::level;
    XX(INFO, INFO)
    XX(DEBUG, DEBUG)
    XX(WARN, WARN)
    XX(ERROR, ERROR)
    XX(FATAL, FATAL)

    XX(INFO, info)
    XX(DEBUG, debug)
    XX(WARN, warn)
    XX(ERROR, error)
    XX(FATAL, fatal)
#undef XX
    return LogLevel::UNKNOWN;
}

void LogEvent::printf(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);  // 将 fmt 后面的参数添加到可变参数列表 ap
    vprintf(fmt, ap);   
    va_end(ap);
}

void LogEvent::vprintf(const char* fmt, va_list ap) {
    char* buf = nullptr;
    int len = vasprintf(&buf, fmt, ap);
    if (len != -1){
        m_ss << std::string(buf, len);
        free(buf);
    }
}


/**
    为每一个格式化项派生一个用于解析到标准输出流的子类，供 LogFormatter 调用解析。具体格式项有：      
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
class MessageFormatItem: public LogFormatter::FormatItem {
public:
    // 当然，大部分格式化项解析类的构造函数没啥用，因为没有成员变量接收构造传入的参数
    // 仍旧要写构造函数的目的只在于保持格式统一方便用宏批量生成对象，因为还是有个别解析类的构造需要参数的
    MessageFormatItem(const std::string& str) {} 
    // 后面的 override 修饰符可以不加，加上的作用是声明这是一个重写函数，
    // 在编译的时候，如果编译器发现该函数不是重写，比如返回值类型或者参数列表和基类虚函数有区别，会报错。  
    void format(std::ostream& os, LogEvent::ptr event) override {
        os << event -> getContent();
    }
};

class LevelFormatItem: public LogFormatter::FormatItem {
public:
    LevelFormatItem(const std::string& str) {}  
    void format(std::ostream& os, LogEvent::ptr event) override {
        os << LogLevel::toString( event -> getLevel());
    }
};

class ElapseFormatItem: public LogFormatter::FormatItem {
public:
    ElapseFormatItem(const std::string& str) {}  
    void format(std::ostream& os, LogEvent::ptr event) override {
        os << event -> getElapse();
    }
};

class LoggerNameFormatItem: public LogFormatter::FormatItem {
public:
    LoggerNameFormatItem(const std::string& str) {}  
    void format(std::ostream& os, LogEvent::ptr event) override {
        os << event -> getLoggerName();
    }
};

class ThreadNameFormatItem: public LogFormatter::FormatItem {
public:
    ThreadNameFormatItem(const std::string& str) {}  
    void format(std::ostream& os, LogEvent::ptr event) override {
        os << event -> getThreadName();
    }
};

class ThreadIdFormatItem: public LogFormatter::FormatItem {
public:
    ThreadIdFormatItem(const std::string& str) {}  
    void format(std::ostream& os, LogEvent::ptr event) override {
        os << event -> getThreadId();
    }
};

class FiberIdFormatItem: public LogFormatter::FormatItem {
public:
    FiberIdFormatItem(const std::string& str) {}  
    void format(std::ostream& os, LogEvent::ptr event) override {
        os << event -> getFiberId();
    }
};

class FileNameFormatItem: public LogFormatter::FormatItem {
public:
    FileNameFormatItem(const std::string& str) {}  
    void format(std::ostream& os, LogEvent::ptr event) override {
        os << event -> getFile();
    }
};

class LineFormatItem: public LogFormatter::FormatItem {
public:
    LineFormatItem(const std::string& str) {}  
    void format(std::ostream& os, LogEvent::ptr event) override {
        os << event -> getLine();
    }
};

class NewLineFormatItem: public LogFormatter::FormatItem {
public:
    NewLineFormatItem(const std::string& str) {}  
    void format(std::ostream& os, LogEvent::ptr event) override {
        os << std::endl;
    }
};

class TabFormatItem: public LogFormatter::FormatItem {
public:
    TabFormatItem(const std::string& str) {}  
    void format(std::ostream& os, LogEvent::ptr event) override {
        os << "\t";
    }
};

class PercentageFormatItem: public LogFormatter::FormatItem {
public:
    PercentageFormatItem(const std::string& str) {}  
    void format(std::ostream& os, LogEvent::ptr event) override {
        os << "%";
    }
};

class StringFormatItem: public LogFormatter::FormatItem {
public:
    // 这里构造函数的 str 有实际的用处了 
    StringFormatItem(const std::string& str): m_str(str) {}  
    void format(std::ostream& os, LogEvent::ptr event) override {
        os << m_str;
    }
private:
    std::string m_str;
};

class DateTimeFormatItem: public LogFormatter::FormatItem {
public:
    DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S"): 
        m_format(format) {
            if (m_format.empty())  m_format = "%Y-%m-%d %H:%M:%S";
        }  

    void format(std::ostream& os, LogEvent::ptr event) override {
        /** tm 是 C/C++ 内置的时间块结构体，以人类友好的格式存储日期时间
        struct tm{
            int tm_sec;                   Seconds.     [0-60] (1 leap second)
            int tm_min;                   Minutes.     [0-59]
            int tm_hour;                  Hours.       [0-23]
            int tm_mday;                  Day.         [1-31]
            int tm_mon;                   Month.       [0-11]
            int tm_year;                  Year - 1900.
            int tm_wday;                  Day of week. [0-6]
            int tm_yday;                  Days in year.[0-365]
            int tm_isdst;                 是否夏令时.    [-1/0/1]       
        }

        time_t 类型是毫秒表示的UTC时间戳，通常人类无法阅读，通过 
        localtime_r(&time_t, &tm) 可以将 time_t 类型变量（取地址）填充到 tm 结构体（取地址）中。      
        strftime(buf, sizeof buf, (char*)format, &tm) 函数可以将结构体 tm（取地址） 中的内容
        按照 format （C风格字符数组表示）格式写入到 长度为 sizeof buf 的C 风格数组 buf 中。     
        */
        struct tm tm;    
        time_t time = event -> getTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof buf, m_format.c_str(), &tm);
        os << buf;
    }

private:
    std::string m_format;
};

LogFormatter::LogFormatter(const std::string& pattern): m_pattern(pattern) {
    init();
}

/**
 遍历 pattern 字符串解析之，对每一个格式描述符或者普通内容生成一个相应的 formatItem 派生类对象
 并将其填入到 解析类对象数组 中  
*/
void LogFormatter::init() {
    // 按顺序解析和存储每一个 pattern 项，使用 pair 的原因是需要一个标志位区别当前 pattern 
    // 是格式化项还是普通内容项。格式化项前面是 1，普通内容是 0  。     
    std::vector<std::pair<int, std::string>> patterns;
    //  临时存储普通内容
    std::string tmp;       
    // 日期项，存储 %d 后面大括号内的内容    
    std::string dateformat; 
    // 出错标志位    
    bool error = false;    
    // 正在解析的对象，也就是解析当前字符之前在解析什么。 true 是普通内容， false 是格式化项      
    bool parsing_string = true;

    size_t i = 0;
    while (i < m_pattern.size()) {
        std::string c(1, m_pattern[i]); // char 字符转为 std::string字符串
        // debug
        // std::cout << "i: " << i << " c: " << c << " ";
        
        if (c == "%") {  
            if (parsing_string) {// 此前正在解析的是普通内容项（或者一个格式化项已结束）
                if (tmp.size()) {   // 临时存储普通内容项的字符串非空
                    patterns.push_back(std::make_pair(0, tmp)); //0 代表非格式化项
                }
                tmp.clear();
                parsing_string = false; // 显然，遇到了 %，要开始解析格式化项了
                i ++;
                continue; 
            }
            // 遇到了百分号，且截止到上一个字符仍然在解析格式化项，由于一个完整的格式化详解析结束后 
            // parsing_string 会置 true，此时只有 %% 一种情况
            else { 
                patterns.push_back(std::make_pair(1, c)); // 1 是格式化项      
                parsing_string = true; // 一个格式化项结束，parsing_string 标志位置 true     
                i++;
                continue;
            }
        }
        else { // 不是 % 
            if (parsing_string){ // 继续添加非格式化的普通内容项
                tmp += c;
                i ++;
                continue;
            }
            else {  // 格式化字符，必然是一个字符，直接添加到 patterns 中，标志位置 true
                patterns.push_back(std::make_pair(1, c));
                parsing_string = true;

                // 日期项需要特别处理，其他格式化项添加到 patterns 结束解析后直接过。
                if (c != "d"){
                    i ++;
                    continue;
                }

                // debug
                // std::cout << "i: " << i << " c: " << c << " ";

                // 则 c == "d"
                i ++;
                if (i < m_pattern.size() && m_pattern[i] != '{'){
                    // 这种情况会发生吗？可能会发生，如果不指定格式，按照构造函数，应该使用缺省格式。  
                    i ++;
                    continue;
                }
                i++; // 否则走到 { 的下一位，开始遍历并向 dateformat 字符串添加日期格式
                while (i < m_pattern.size() && m_pattern[i] != '}'){
                    // std::cout << i << ": " << m_pattern[i] << " ";
                    dateformat.push_back(m_pattern[i]);
                    i++;
                }
                if (i == m_pattern.size() || m_pattern[i] != '}'){
                    // %d 后面的大括号没有闭合，报错退出。但是按照上面的 while 语句，只要没有 } 
                    // 就一直往后走，最终应该走到结尾，也即 i 越界。加上 || 后面一句判断保险一点。      
                    std::cout << "[ERROR] LogFormatter::init() " << "pattern: [" <<
                    m_pattern << "] '{' not closed" << std::endl;
                    error = true;
                    break;
                }
                // 正常走完大括号。
                i ++;
                continue;
            }
        }
    }

    if (error){
        m_error = true;
        return ;
    }

    // 模式解析结束后可能有普通的内容项没有加入到 patterns 
    if (tmp.size()) {
        patterns.push_back(std::make_pair(0, tmp));
        tmp.clear();
    }

    // std::function<type_return(type_args)> c++11 引入的函数封装器
    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)>> s_format_items = {
#define XX(str, C) {#str, [](const std::string& fmt) {return FormatItem::ptr(new C(fmt));} } 
        XX(m, MessageFormatItem),           // %m   消息体，事件内容
        XX(p, LevelFormatItem),             // %p   日志级别
        XX(c, LoggerNameFormatItem),        // %c   日志器名
        XX(r, ElapseFormatItem),            // %r   日志器启动至今的时间
        XX(l, LineFormatItem),              // %l   行号
        XX(f, FileNameFormatItem),          // %f   文件名
        XX(t, ThreadIdFormatItem),          // %t   线程 ID
        XX(F, FiberIdFormatItem),           // %F   协程 ID
        XX(N, ThreadNameFormatItem),        // %N   线程名
        XX(T, TabFormatItem),               // %T   tab
        XX(%, PercentageFormatItem),        // %%   百分号
        XX(n, NewLineFormatItem),           // %n   换行
        XX(d, DateTimeFormatItem)           // %d   日期时间
#undef XX
    };

    for (auto& v: patterns) {
        if (v.first == 0) {
            m_items.push_back(FormatItem::ptr(new StringFormatItem(v.second)));
        }
        else {
            auto it = s_format_items.find(v.second);
            if (it == s_format_items.end()){
                std::cout << "[ERROR] LogFormatter::init() " << "pattern: [" << m_pattern 
                << "] unknown format item: " << v.second << std::endl;
                // std::cout << dateformat << std::endl;
                error = true;
                break;
            }
            else {
                if (it -> first == "d") m_items.push_back(it -> second(dateformat));
                else m_items.push_back(it -> second(""));
            }
        }
    }

    if (error) {
        m_error = true;
        return ;
    }

}

std::string LogFormatter::format(LogEvent::ptr event) {
    std::stringstream ss;
    for (auto &it: m_items) it -> format(ss, event);
    return ss.str();
}

// 标准输出流是个基类，可以接收 cout 也可以接受 stringstream 指针。  
void LogFormatter::format(std::ostream& os, LogEvent::ptr event) {
    for (auto &it: m_items) it -> format(os, event);
}


LogAppender::LogAppender(LogFormatter::ptr default_formatter): 
    m_default_formatter(default_formatter), m_formatter(nullptr) {
}

void LogAppender::setFormatter(LogFormatter::ptr formatter) {
    // 次出访至线程竞争应当先加锁再赋值。但我们先实现基础功能，线程安全以后再实现
    m_formatter = formatter;
}

LogFormatter::ptr LogAppender::getFormatter() {
    // 同样也应该先加锁再返回，但是先实现基础功能。
    return m_formatter ? m_formatter : m_default_formatter;
}

StdoutLogAppender::StdoutLogAppender(): 
    LogAppender(LogFormatter::ptr(new LogFormatter)){
}

void StdoutLogAppender::log(LogEvent::ptr event) {
    if (m_formatter)    m_formatter -> format(std::cout, event);
    else        m_default_formatter -> format(std::cout, event);
}

bool FileLogAppender::reopen() { 
    // 也应该加锁
    if (m_filestream) m_filestream.close();
    m_filestream.open(m_filename, std::ios::app); // 追加模式打开向 m_filename 的标准输出流
    m_reopenError = !m_filestream;
    return !m_reopenError; // 返回失败标志位的非
}

FileLogAppender::FileLogAppender(const std::string& filename): 
    LogAppender(LogFormatter::ptr(new LogFormatter)){
    m_filename = filename;
    reopen();
    if (m_reopenError)
        std::cout << "reopen file " << m_filename << " error" <<std::endl;
}

void FileLogAppender::log(LogEvent::ptr event) {
    uint64_t now = event -> getTime();
    if (now >= m_lastTime+3) {
        reopen();
        m_lastTime = now;
        if (m_reopenError) {
            std::cout << "reopen file " << m_filename << " error" <<std::endl;
            return;
        }
    }

    // 要加锁，锁还没有实现

    if (m_formatter)    m_formatter -> format(m_filestream, event);
    else        m_default_formatter -> format(m_filestream, event);
    if (!m_filestream)  
        std::cout << "[ERROR] FileLogAppender::log() format error" << std::endl;
}

Logger::Logger(const std::string& name): m_name(name) {
    m_createTime = time(NULL);
}

void Logger::addAppender(LogAppender::ptr appender) {
    // 加锁
    m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender) {
    // 加锁 
    for (auto it = m_appenders.begin(); it != m_appenders.end(); it ++){
        if (*it == appender){
            m_appenders.erase(it);
            break;
        }
    }
}

void Logger::clearAppender() {
    // 加锁
    m_appenders.clear();
}

void Logger::log(LogEvent::ptr event) {
    // 加锁
    // 只有当事件级别不小于日志器级别的时候，才能输出到每个 m_appenders
    if (event -> getLevel() >= m_level)
        for (auto& appender: m_appenders ) 
            appender -> log(event);
}

LogEventWarp::LogEventWarp(Logger::ptr logger, LogEvent::ptr event):
    m_logger(logger), m_event(event){
}

// 包装器析构的时候写日志
LogEventWarp::~LogEventWarp(){
    m_logger -> log(m_event);
}

LoggerManager::LoggerManager() {
    m_root.reset(new Logger("root")); // shared_ptr 智能指针内置方法？应该是
    m_root -> addAppender(LogAppender::ptr(new StdoutLogAppender()));
    m_loggers[m_root -> getName()] = m_root;
    // init();
}

Logger::ptr LoggerManager::getLogger(const std::string& name) {
    // 加锁
    auto it = m_loggers.find(name);
    if (it != m_loggers.end())  return it -> second;

    Logger::ptr logger(new Logger(name));
    m_loggers[name] = logger;
    return logger;
}

} // end liux
