#include <utility> // std::pair

#include "log.h"


namespace sylar{

const char* LogLevel::toString(LogLevel::Level level){
    switch (level): {
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
#define XX(level, v) if (v == #level) return LogLevel::level;
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
    LoggerNameFormatItem(const std::string& str) {}  
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

class LoggerNameFormatItem: public LogFormatter::FormatItem {
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
    DateTimeFormatItem(const std::string& pattern = "%Y-%m-%d %H:%M:%S"): 
        m_pattern(pattern) {
            if (m_pattern.empty())  m_pattern = "%Y-%m-%d %H:%M:%S";
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
        strftime(buf, sizeof buf, m_formatter.c_str(), &tm);
        os << buf;
    }

private:
    std::string m_pattern;
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
    std::vector<std::pait<int, std::string>> patterns;
    //  临时存储普通内容
    std::string tmp;       
    // 日期项，存储 %d 后面大括号内的内容    
    std::string dateformat; 
    // 出错标志位    
    bool error = false;    
    // 正在解析的对象，也就是解析当前字符之前在解析什么。 true 是普通内容， false 是格式化项      
    bool parsing_string
    
}


}