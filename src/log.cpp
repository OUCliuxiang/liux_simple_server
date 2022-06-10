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

}