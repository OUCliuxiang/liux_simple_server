// 日志模块，包括写日志和一些文件相关、时间相关函数。不用类封装，没必要。
// 用命名空间给它包起来，使用的时候 log::func 即可

#ifndef __LOG_H__
#define __LOG_H__

#include <string>
#include <vector>
#include <tuple>
#include <time.h>

#include "util.h"

// Level_FATAL
#define LFATAL      5
#define LERROR      4
#define LWARN       3
#define LINFO       2
#define LVERBOSE    1

#define FATAL log::__log(__FILE__, __LINE__, LFATAL, __VA_ARGS__)
#define ERROR log::__log(__FILE__, __LINE__, LERROR, __VA_ARGS__)
#define WARN log::__log(__FILE__, __LINE__, LWARN, __VA_ARGS__)
#define INFO log::__log(__FILE__, __LINE__, LINFO, __VA_ARGS__)
#define VERBOSE log::__log(__FILE__, __LINE__, LVERBOSE, __VA_ARGS__)

namespace log {

    using namespace std;

    string date_now(); 
    string time_now();
    string gmtime_now();
    string gmtime(time_t t);
    time_t gmtime2ctime(const string& gmt);
    void sleep(int ms);
    long long timestamp_now();
    
    
    bool isfile(const string& file);
    bool mkdir(const string& path);
    bool mkdirs(const string& path);
    bool exists(const string& path);

    FILE* fopen_mkdirs(const string& path, const string& mode);
    string file_name(const string& path, bool include_suffix);
    string directory(const string& path);

    time_t last_modify(const string& file);
    vector<uint8_t> load_file(const string& file);
    string load_text_file(const string& file);
    size_t file_size(const string& file);

    bool begin_with(const string& str, const string& with);
    bool end_with(const string& str, const string& with);
    vector<string> split_string(const string& str, const string& spstr);
    string replace_string(const string& str, const string& token, const string& value);
    bool pattern_match(const string& str, const string& matcher, bool ignore_cast = true);
    
    vector<string> find_files(
        const stirng& directory,
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
    void __log(const char* file, int line, int level, const char* fmt, ...);
    void destroy_logger(); // 销毁日志器
}


#endif // __LOG_H__