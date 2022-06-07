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
    m_fp = fopen(m_filename, "a"); // a 追加模式，若后面带 +： 读写模式，不带则只写    
    return !!m_fp;
}

void FileLogAppender::log(LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level)  fprintf(m_fp, "%s", m_formatter.format(event));
    // 不要用 fwrite，这个是二进制写，我们希望写入格式化为字符串后的日志内容。    
    // fwrite 写入用 fread 读，fprintf 写入用 fscanf 读。       
    fclose();
}

void StdoutLogAppender::log(LogLevel::Level level, LogEvent::ptr event){
    if (level >= m_level) fprintf(stdout, "%s", m_formatter.format(event));
}

}