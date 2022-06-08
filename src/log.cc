#include "log.h"
#include <map>

namespace sylar{

const char* LogLevel::toString(Level level) const {
    switch (level) {
    case DEBUG: return "DEBUG"; break;
    case INFO: return "INFO"; break;
    case WARN: return "WARN"; break;
    case ERROR: return "ERROR"; break;
    case FATAL: return "FATAL"; break;
    default: return "UNKNOWN"; break;
    }
    return "UNKNOWN";
}

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

bool FileLogAppender::reopen(){
    if (m_fp) fclose(m_fp);
    m_fp = fopen(m_filename.c_str(), "a"); // a 追加模式，若后面带 +： 读写模式，不带则只写    
    return !!m_fp;
}

void FileLogAppender::log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) {
    if (reopen()) {
        if (level >= m_level)  fprintf(m_fp, "%s", m_formatter.format(logger, level, event).c_str());
        // 不要用 fwrite，这个是二进制写，我们希望写入格式化为字符串后的日志内容。    
        // fwrite 写入用 fread 读，fprintf 写入用 fscanf 读。       
        fclose(m_fp);
    }   
    else {   
        fprintf(stderr, "%s: open failed.\n", m_filename.c_str());
    }
}

void StdoutLogAppender::log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event){
    if (level >= m_level) fprintf(stdout, "%s", m_formatter.format(logger, level, event).c_str());
}

std::string LogFormatter::format(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event){
    std::stringstream ss;
    for (auto& i: m_items)  i -> format(ss, logger, level, event);
    return ss.str();
}

/** 按照百分号区分不同类型信息, 三种格式：      
    %xxx, 直接指定某类型格式      
    %xxx{xxx}, 携带具体格式参数，如 %d{%Y-%M-%D %h: %m: %s: %ms}   
    %% 转义，输出百分号         
**/
void LogFormatter::init(){
    // string 内容, format 格式, type 具体类型分为： 0: 普通字符， 1: 格式描述符     
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
                && m_pattern != '}') { // 遇到非（法）格式描述符比如空格，退出
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
            if (fmt_status == 1 && m_pattern[n] == '}'){ // 读到大括号后首次读到右括号：参数结束
                fmt = m_pattern.substr(fmt_begin, n - fmt_begin);
                fmt_status = 0; // 含参格式解析结束
                ++ n;
                break; // 本组描述符完全结束，退出。 
            }
            if (++ n == m_pattern.size()){ // 格式描述符在 m_pattern 字符串最后的情况
                if (str.empty()) 
                    str = m_pattern.substr();
            }
        }

        if (fmt_status == 0) { // 没有读到大括号或完整读取到左右括号
            if (nstr.size()) {
                vec.push_back(std::make_tuple(nstr, "", 0)); 
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1)); // 1 代表有含义的格式描述符
            i = n-1;     
        }else { // 括号没读完，错误     
            m_error = true;
            fprintf(stderr, "pattern parse error: %s: - %s\n", m_pattern, m_pattern.substr(i));
            vec.push_back(std::make_tuple("<pattern error>", fmt, 0)); // 由于参数错误，无法解析，type = 0
            i = n-1;
        }
    }
    
    if (nstr.size()) vec.push_back(std::make_tuple(nstr, "", 0));

    /** 
    %m -- 消息体
    %p -- level 
    %r -- 启动后的时间
    %c -- 日志名称  
    %t -- 线程 id 
    %d -- 时间
    %n -- 换行
    %f -- 文件名  
    %l -- 行号
    **/

    // std::function 是 c++11 引入的函数封装器。封装一个构造函数返回一个特定对象。
    static std::map<std::string, std::function<FormatItem::ptr(const string& str)>> s_format_items = {
    // 变量名前面加 # 将宏参数转成一个字符串
#define XX(str, C) {#str, [](const std::string& fmt){return FormatItem::ptr(new C(fmt));}} 
        XX(m, MessageFormatItem),
        XX(p, LevelFormatItem),
        XX(r, ElapseFormatItem),
        XX(c, NameFormatItem),
        XX(t, ThreadIdFormatItem),
        XX(d, DataTimeFormatItem),
        XX(n, NewLineFormatItem)
        XX(f, FilenameFormatItem),
        XX(l, LineFormatItem)
#undef XX
    };

    for (auto& i: vec){
        if (std::get<2>(i) == 0)  // 0: 普通字符
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        else { // 1: 有含义的格式描述符
            const auto& it = s_format_items.find(std::get<0>(i)); // 获取到迭代器
            if (it == s_format_items.end()){
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<< error fromat %" + std::get<0>(i) + ">>")));
                m_error = true;
            } else {
                m_items.push_back(it -> second(std::get<0>(1))); // 1 是fmt 是括号中的内容，这，？？
            }
        }

        fprintf(stdout, "%s - %s - %d \n", std::get<0>(i).c_str(), std::get<1>(i).c_str(), std::get<2>(i));
    }
}


class MessageFormatItem: public LogFormatter::FormatItem {
public: 
    void format(std::stringstream& ss, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
        ss << event -> getContent();
    }
};

class LevelFormatItem: public LogFormatter::FormatItem {
public: 
    void format(std::stringstream& ss, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
        ss << event -> LogLevel::toString(level);
    }
};

class ElapseFormatItem: public LogFormatter::FormatItem {
public: 
    void format(std::stringstream& ss, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
        ss << event -> getElapse();
    }
};

class NameFormatItem: public LogFormatter::FormatItem {
public: 
    void format(std::stringstream& ss, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
        ss << logger -> getName();
    }
};

class ThreadIdFormatItem: public LogFormatter::FormatItem {
public: 
    void format(std::stringstream& ss, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
        ss << event -> getThreadId();
    }
};

class FiberIdFormatItem: public LogFormatter::FormatItem {
public: 
    void format(std::stringstream& ss, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
        ss << event -> getFiberId();
    }
};

class DataTimeFormatItem: public LogFormatter::FormatItem {
public: 
    DataTimeFormatItem(const std::string& format = "%Y:%m:%d %H:%M:%S"): m_format(format) {}
    void format(std::stringstream& ss, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
        ss << event -> getTime();
    }
private:
    std::string m_format;
};

class StringFormatItem: public LogFormatter::FormatItem {
public: 
    StringFormatItem(const std::string& str = ""): m_str(str) {}
    void format(std::stringstream& ss, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
        ss << m_str;
    }
private:
    std::string m_str;
};

class FilenameFormatItem: public LogFormatter::FormatItem {
public: 
    void format(std::stringstream& ss, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
        ss << event -> getFile();
    }
};

class LineFormatItem: public LogFormatter::FormatItem {
public: 
    void format(std::stringstream& ss, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
        ss << event -> getLine();
    }
};

class NewLineFormatItem: public LogFormatter::FormatItem {
public: 
    void format(std::stringstream& ss, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
        ss << "\n";
    }
};

}