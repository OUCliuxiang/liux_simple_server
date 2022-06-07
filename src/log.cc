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
    %xxx{xxx}, 携带具体格式参数，如 %d{%Y-%M-%D %h: %m: %s: %ms}   
    %% 转义，输出百分号         
**/
void LogFormatter::init(){
    // string 内容, format 格式, type 具体类型
    std::vector<std::tuple<std::string,, std::string, int>> vec;
    std::string nstr; // 存放普通的非格式化描述符内容
    for (size_t i = 0; i < m_pattern.size(); i++){
        
        if (m_pattern[i] != '%') {
            // 如果不是 % ，应当是常规的字符串内容，如 "nums is %d" 中的 "nums is "
            nstr.append(1, m_pattern[i]);
            continue;
        }

        if (i+1 < m_pattern.size() && m_pattern[i+1] == '%') { // %%
            nstr.append(1, '%');
            continue;
        }

        // 排除前面两种情况，此时一定是 %xxx 或 %xx{yy} 情形
        size_t n = i + 1;  // 格式描述符开始的位置    
        size_t fmt_begin = 0; // 带参结构参数开始的位置     
        int fmt_status = 0; // 0: 尚未读到大括号  1: 读到左大括号  2: 读到右大括号         
        

        // 应对含参格式如 %xx{yy}, str 存储格式 xx, fmt 存储参数 yy
        std::string str; 
        std::string fmt;
        
        while (n < m_pattern.size()) { // 当前 % 后面的格式描述符（组合）
            
            if (!fmt_status && !isalpha(m_pattern[n]) && m_pattern[n] != '{' 
                && m_pattern != '}') { // 遇到非（法）格式描述符，退出
                str.append(i+1, n-i-1);
                break;
            }    
            
            if (!fmt_status && m_pattern[n] == '{'){ // 格式描述符后面首次读到大括号：有参格式
                str = m_pattern.substr(i+1, n-i-1);
                ++ n;
                fmt_status = 1; 
                fmt_begin = n;
                continue;
            }
            else if (fmt_status == 1 && m_pattern[n] == '}'){ // 读到大括号后首次读到右括号：参数结束
                fmt = m_pattern.substr(fmt_begin, n - fmt_begin);
                fmt_status = 2; // 含参格式解析结束
                ++ n;
                break; // 本组描述符完全结束，退出。 
            }
            ++ n;
        }

        if (fmt_status == 0) {
            str = m_pattern.substr()
        }
    }
}

}