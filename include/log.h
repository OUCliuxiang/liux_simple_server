// 日志模块，包括写日志和一些文件相关、时间相关函数。不用类封装，没必要。
// 用命名空间给它包起来，使用的时候 log::func 即可

#ifndef __LOG_H__
#define __LOG_H__

#include <string>
#include <vector>
// #include <tuple>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h> // syscall
#include <sys/syscall.h> //SYS_gettid


// Level_FATAL
#define LFATAL      5
#define LERROR      4
#define LWARN       3
#define LINFO       2
#define LVERBOSE    1

#define FATAL(...)   Log::__log(__FILE__, __LINE__, LFATAL, __VA_ARGS__)
#define ERROR(...)   Log::__log(__FILE__, __LINE__, LERROR, __VA_ARGS__)
#define WARN(...)    Log::__log(__FILE__, __LINE__, LWARN, __VA_ARGS__)
#define INFO(...)    Log::__log(__FILE__, __LINE__, LINFO, __VA_ARGS__)
#define VERBOSE(...) Log::__log(__FILE__, __LINE__, LVERBOSE, __VA_ARGS__)

namespace Log {
    // 命名空间中不要写实现，否则编译会报 multiple defination 的错误

    using namespace std;

    string date_now(); 
    string time_now();
    string gmtime_now(); 
    string gmtime(time_t t); // UTC 格式时间，具体在 cpp 里有例子
    time_t gmtime2ctime(const string& gmt); // 转为自1970经过的秒数
    void sleep(int ms);
    long long timestamp_now(); // 获取时间戳
    
    
    bool isfile(const char* file);
    bool mkdir(const char* path);
    bool mkdirs(const char* path);
    bool exists(const char* path);


    FILE* fopen_mkdirs(const char* path, const char* mode);
    string file_name(const string& path, bool include_suffix);
    string directory(const string& path);


    time_t last_modify(const char* file);
    vector<uint8_t> load_file(const char* file);
    string load_text_file(const string& file);
    size_t file_size(const string& file);


    bool begin_with(const char* str, const char* with);
    bool end_with(const char* str, const char* with);
    vector<string> split_string(const string& str, const string& spstr);
    string replace_string(const string& str, const string& token, const string& value);
    bool pattern_match(const char* str, const char* matcher, bool ignore_cast = true);
    

    vector<string> find_files(
        const string& directory,
        const string& filter = "*", 
        bool findDirectory = false,
        bool includeSubDirectory = false
    );
    string align_blank(const string& input, int align_size, char blank = ' ');


    bool save_file(const string& file, const vector<uint8_t>& data, bool mk_dirs = true);
    bool save_file(const string& file, const string& data, bool mk_dirs = true);
    bool save_file(const string& file, const void* data, size_t len, bool mk_dirs = true);


    // 循环等待， 并捕获 ctrl+C 这类的终止信号，收到信号后结束循环并返回信号类型
    int while_loop();

    string format(const char* fmt, ...);


    const char* log_level(int level);
    void set_logger_save_directory(const string& directory);
    void set_log_level(int level); // 过滤，高于这个级别的日志才输出
    // 日志输出函数，在上面写成了宏以方便实用
    void __log(const string& file, int line, int level, const char* fmt, ...);
    void destroy_logger(); // 销毁日志器

    
    pid_t GetThreadId();
    uint64_t GetCurrentMS();
    uint64_t GetCurrentUS();
}


#endif // __LOG_H__