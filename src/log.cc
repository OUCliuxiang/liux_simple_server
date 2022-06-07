#include "log.h"

namespace sylar{

Logger(const std::string& name = "root"): m_name(name){
}

void Logger::addAppender(LogAppender::ptr appender){
    m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender){
    for (auto it = m_appenders.begin(); it != m_appenders.end(); ++it)
        // erase 删除， 一带要用迭代器
        if (*it == appender) {
            m_appenders.erase(it);
            break;
        }
}

void Logger::log(LogLevel::Level level, LogEvent::ptr event){
    if (level >= m_level) 
        for (auto& i : m_appenders)
            i -> log(event);
}

void Logger::debug(LogEvent::ptr event){
    log(LogLevel::DEBUG, event);
}

void Logger::info(LogEvent::ptr event){
    log(LogLevel::INFO, event);
}

void Logger::warn(LogEvent::ptr event){
    log(LogLevel::WARN, event);
}

void Logger::error(LogEvent::ptr event){
    log(LogLevel::ERROR, event);
}

void Logger::fatal(LogEvent::ptr event){
    log(LogLevel::FATAL, event);
}

void FileLogAppender::reopen(){
    if (m_fp) fclose(m_fp);
    m_fp = fopen(m_filename.c_str(), "a"); // a 追加模式，若后面带 +： 读写模式，不带则只写    
    return !!m_fp;
}

void FileLogAppender::log(LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level)  fprintf(m_fp, "%s", m_formatter.format(event).c_str());
    // 不要用 fwrite，这个是二进制写，我们希望写入格式化为字符串后的日志内容。    
    // fwrite 写入用 fread 读，fprintf 写入用 fscanf 读。       
    fclose();
}

void StdoutLogAppender::log(LogLevel::Level level, LogEvent::ptr event){
    if (level >= m_level) fprintf(stdout, "%s", m_formatter.format(event));
}

std::string LogFormatter::format(LogEvent::ptr event){
    std::stringstream ss;
    for (auto& i: m_items)  i -> format(ss, event);
    return ss.str();
}

/** 按照百分号区分不同类型信息, 三种格式：      
    %xxx, 直接指定某类型格式      
    %xxx{xxx}, 携带具体格式参数，具体例子不明确。   
    %% 转义，输出百分号         
**/
void LogFormatter::init(){
    // string 内容, format 格式, type 具体类型
    std::vector<std::tuple<std::string,, std::string, int>> vec;
    std::string str; // format 格式 字符串形式
    for (size_t i = 0; i < m_pattern.size(); i++){
        if (m_pattern[i] != '%') {
            str.append(1, m_pattern[i]);
            continue;
        }
        // 走过 % 后 i 一定是 % 后面第一个字符
        size_t n = i + 1;
        int fmt_status = 0; // 是否读到大括号，读到置一，说明后续读的内容是格式的具体参数。     
        size_t fmt_begin = 0; // 带参结构参数开始的位置。 

        std::string fmt, str;
        
        while (n < m_pattern.size()) {
            if (isspace(m_pattern[n]))  break;
            // 如果是空格，说明该类型描述终止，可以继续读下一个类型描述符如 "%d "
            
            if (fmt_status == 0){
                if (m_pattern[n] == '{'){ // 有具体类型参数 的描述符
                    str = m_pattern.substr(i, n-i);
                    ++ n;
                    fmt_status = 1; 
                    fmt_begin = n;
                    continue;
                }
            }
            if (fmt_status == 1){ // 已经读到大括号，后面的是具体格式参数
                if (m_pattern[n] == '}'){
                    fmt = m_pattern.substr(fmt_begin, n - fmt_begin);
                }
            }
        }
    }
}

}