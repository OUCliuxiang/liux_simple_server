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
    va_start(ap, fmt); // 将 fmt 后面的参数添加到可变参数列表 ap
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
    // 当然，大部分格式化项解析类的构造函数没啥用，因为没有成员变量接收构造传入的参数
    // 仍旧要写构造函数的目的只在于保持格式统一方便用宏生成对象，因为还是有个别解析类的构造需要参数的
    MessageFormatItem(const std::string& str) {} 
    // 后面的 override 修饰符可以不加，加上的作用是声明这是一个重写函数，
    // 在编译的时候，如果编译器发现该函数不是重写，比如返回值类型或者参数列表和基类虚函数有区别，会报错。  
    void format(std::ostream& os, LogEvent::ptr event) override {
        os << event -> getContent();
    }
}

}