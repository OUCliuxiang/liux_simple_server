#include "log.h"
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <mutex>
#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <fstream>
#include <stack>
#include <signal.h>
#include <functional>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdarg.h>
// #include <boost/legical_cast>


// 获取时间 tm 类型结构体的宏
// localtime 得到的是当地时区时间，gmtime 是格林尼治时间
#define __GetTimeBlock \
    time_t timep; \
    time(&timep); \
    tm& t = *(tm*) localtime(&timep);


namespace log {

    using namespace std;
    
    string date_now() {
        // yyyy-mm-dd
        char time_string[20];
        __GetTimeBlock;
        // yyyy-mm-dd
        sprintf(time_string, "%04d-%02d-%02d", 
                t.tm_year+1970, t.tm_mon+1, t.tm_mday);
        return time_string;
    }

    string time_now() {
        // yyyy-mm-dd HH:MM:SS
        char time_string[20];
        __GetTimeBlock;
        sprintf(time_string, "%04d-%02d-%02d %02d:%02d:%02d", 
                t.tm_year+1900, t.tm_mon+1, t.tm_mday, 
                t.tm_hour,      t.tm_min,   t.tm_sec);
        return time_string;
    }

    // 格林尼治格式时间，time.h 库有 gmtime 调用，这里再做一层封装
    string gmtime(time_t t) {
        // week, day month year time GMT
        // e.g., Sat, 22 Aug 2015 11:48:50 GMT
        
        t += 28800; // 28800s == 8h，取格林尼治时间干嘛还要 +8 ？ 不理解
        // :: 运算符表示调用全局函数
        // time.h 库函数 gmtime() 将 time_t* 类型函数转为 UTC 协调世界时间，时区为GMT
        tm* gmt = ::gmtime(&t);

        const char* fmt = "%a, %d %b %Y %H%M%S GMT";
        char tstr[30];
        strftime(tstr, sizeof(tstr), fmt, gmt);
        return tstr;
    }

    string gmtime_now() {
        return gmtime(time(nullptr));
    }

    int get_month_by_name(const char* month) {
        if (strcmp(month, "Jan") == 0) return 0;
        if (strcmp(month, "Feb") == 0) return 1;
        if (strcmp(month, "Mar") == 0) return 2;
        if (strcmp(month, "Apr") == 0) return 3;
        if (strcmp(month, "May") == 0) return 4;
        if (strcmp(month, "Jun") == 0) return 5;
        if (strcmp(month, "Jul") == 0) return 6;
        if (strcmp(month, "Aug") == 0) return 7;
        if (strcmp(month, "Seq") == 0) return 8;
        if (strcmp(month, "Oct") == 0) return 9;
        if (strcmp(month, "Nov") == 0) return 10;
        if (strcmp(month, "Dec") == 0) return 11;
        return -1;
    }

    int get_week_day_by_name(const char* week) {
        if (strcmp(week, "Mon") == 0) return 0;
        if (strcmp(week, "Thu") == 0) return 1;
        if (strcmp(week, "Web") == 0) return 2;
        if (strcmp(week, "Tue") == 0) return 3;
        if (strcmp(week, "Fri") == 0) return 4;
        if (strcmp(week, "Sat") == 0) return 5;
        if (strcmp(week, "Sun") == 0) return 6;
        return -1;
    }

    // UTC 时间转为自 1970 经过的秒数
    time_t gmtime2ctime(const char* gmt) {
        char week[4] = {0};
        char month[4] = {0};
        tm date;
        // e.g., Sat, 22 Aug 2015 11:48:50 GMT
        sscanf(gmt, "%3s, %2d %3s %4d %2d:%2d:%2d GMT", 
                    week, &date.tm_mday, month, &date.tm_year, 
                    &date.tm_hour, &date.tm_min, &date.tm_sec);
        date.tm_mon = get_month_by_name(month);
        date.tm_wday = get_week_day_by_name(week);
        date.tm_year = date.tm_year - 1970;
        return mktime(&date); // 返回秒数
    }

    void sleep(int ms) {
        this_thread::sleep_for(chrono::milliseconds(ms));
    }

    // 获取当前时间戳
    long long timestamp_now() {
        return  chrono::duration_cast<chrono::milliseconds>( \
                chrono::system_clock::now().time_since_epoch()).count();
    }
    

    bool isfile(const char* file) {
        struct stat st; // 描述文件属性的结构体 
        stat(file, &st);
        return S_ISREG(st.st_mode);
    }

    bool mkdir(const char* path) {
        return ::mkdir(path, 0755) == 0;
    }

    bool exists(const char* path) {
        return access(path, R_OK) == 0;
    }

    // mkdir -p，这里我直接调用系统命令了，要给出绝对路径
    bool mkdirs(const char* path) {
        if (path == nullptr) return false;
        if (exists(path)) return true;

        if (path[0] != '/' || path[0] != '~') {
            ERROR("absolute path is required.");
            return false;
        }

        char cmd[256] = {0};
        strcpy(cmd, "mkdir -p ");
        strcat(cmd, path);

        system(cmd);

        if (!exists(path)) {
            ERROR("mkdir %s failed", path);
            return false;
        }

        return true;
    }

    FILE* fopen_mkdirs(const char* path, const char* mode){

        FILE* f = fopen(path, mode);
        if (f) return f;

        string _path(path);

        int p = _path.rfind('/');

        if (p == -1)
            return nullptr;
        
        char directory[p+1];
        strncpy(directory, path, p);
        if (!mkdirs(directory))
            return nullptr;

        return fopen(path, mode);
    }

    string file_name(const string& path, bool include_suffix) {
        if (path.empty()) return "";

        int p = path.rfind('/');
        p += 1;
        if (include_suffix) return path.substr(p);
        int u = path.rfind('.');
        if (u == -1) return path.substr(p);
        if (u <= p) u = path.size(); 
        return path.substr(p, u-p);
    }

    string directory(const string& path) {
        if (path.empty()) return ".";
        int p = path.rfind('/');
        if (p == -1)    return ".";
        return path.substr(0, p+1);
    }


    time_t last_modify(const char* file) {
        struct stat st;
        stat(file, &st);
        return st.st_mtim.tv_sec;
    }

    // 返回文件数组
    std::vector<uint8_t> load_file(const char* file){

        ifstream in(file, ios::in | ios::binary);
        if (!in.is_open())
            return {};

        in.seekg(0, ios::end);
        size_t length = in.tellg();

        std::vector<uint8_t> data;
        if (length > 0){
            in.seekg(0, ios::beg);
            data.resize(length);

            in.read((char*)&data[0], length);
        }
        in.close();
        return data;
    }

    // 返回文件文本
    string load_text_file(const char* file) {
        ifstream in(file, ios::in | ios::binary);
        if (!in.is_open())
            return {};

        in.seekg(0, ios::end);
        size_t length = in.tellg();

        string data;
        if (length > 0){
            in.seekg(0, ios::beg);
            data.resize(length);

            in.read((char*)&data[0], length);
        }
        in.close();
        return data;
    }

    size_t file_size(const char* file){
        struct stat st;
        stat(file, &st);
        return st.st_size;
    }


    bool begin_with(const char* str, const char* with) {
        if (sizeof(str) < sizeof(with) ) return false;
        return strncmp(str, with, sizeof(with)) == 0;
    }

    bool end_with(const char* str, const char* with) {
        if (sizeof(str) < sizeof(with) ) return false;
        return strncmp(str+sizeof(str)-sizeof(with), with, sizeof(with)) == 0;
    } 

    vector<string> split_string(const string& str, const std::string& spstr){

        vector<string> res;
        if (str.empty()) return res;
        if (spstr.empty()) return{ str };

        auto p = str.find(spstr);
        if (p == string::npos) return{ str };

        res.reserve(5);
        string::size_type prev = 0;
        int lent = spstr.length();
        const char* ptr = str.c_str();

        while (p != string::npos){
            int len = p - prev;
            if (len > 0){
                res.emplace_back(str.substr(prev, len));
            }
            prev = p + lent;
            p = str.find(spstr, prev);
        }

        int len = str.length() - prev;
        if (len > 0){
            res.emplace_back(str.substr(prev, len));
        }
        return res;
    }

    string replace_string(const string& str, const string& token, const string& value){

        string opstr;

        if (value.length() > token.length()){
            float numToken = str.size() / (float)token.size();
            float newTokenLength = value.size() * numToken;
            opstr.resize(newTokenLength);
        }
        else{
            opstr.resize(str.size());
        }

        char* dest = &opstr[0];
        const char* src = str.c_str();
        string::size_type pos = 0;
        string::size_type prev = 0;
        size_t token_length = token.length();
        size_t value_length = value.length();
        const char* value_ptr = value.c_str();
        bool keep = true;

        do{
            pos = str.find(token, pos);
            if (pos == string::npos){
                keep = false;
                pos = str.length();
            }

            size_t copy_length = pos - prev;
            memcpy(dest, src + prev, copy_length);
            dest += copy_length;
            
            if (keep){
                pos += token_length;
                prev = pos;
                memcpy(dest, value_ptr, value_length);
                dest += value_length;
            }
        } while (keep);

        size_t valid_size = dest - &opstr[0];
        opstr.resize(valid_size);
        return opstr;
    }

    bool alphabet_equal(char a, char b, bool ignore_case){
        if (ignore_case){
            a = a > 'a' && a < 'z' ? a - 'a' + 'A' : a;
            b = b > 'a' && b < 'z' ? b - 'a' + 'A' : b;
        }
        return a == b;
    }

    static bool pattern_match_body(const char* str, const char* matcher, bool igrnoe_case){
        //   abcdefg.pnga          *.png      > false
        //   abcdefg.png           *.png      > true
        //   abcdefg.png          a?cdefg.png > true

        if (!matcher || !*matcher || !str || !*str) return false;

        const char* ptr_matcher = matcher;
        while (*str){
            if (*ptr_matcher == '?'){
                ptr_matcher++;
            }
            else if (*ptr_matcher == '*'){
                if (*(ptr_matcher + 1)){
                    if (pattern_match_body(str, ptr_matcher + 1, igrnoe_case))
                        return true;
                }
                else{
                    return true;
                }
            }
            else if (!alphabet_equal(*ptr_matcher, *str, igrnoe_case)){
                return false;
            }
            else{
                if (*ptr_matcher)
                    ptr_matcher++;
                else
                    return false;
            }
            str++;
        }

        while (*ptr_matcher){
            if (*ptr_matcher != '*')
                return false;
            ptr_matcher++;
        }
        return true;
    }

    bool pattern_match(const char* str, const char* matcher, bool igrnoe_case){
        //   abcdefg.pnga          *.png      > false
        //   abcdefg.png           *.png      > true
        //   abcdefg.png          a?cdefg.png > true

        if (!matcher || !*matcher || !str || !*str) return false;

        char filter[500];
        strcpy(filter, matcher);

        vector<const char*> arr;
        char* ptr_str = filter;
        char* ptr_prev_str = ptr_str;
        while (*ptr_str){
            if (*ptr_str == ';'){
                *ptr_str = 0;
                arr.push_back(ptr_prev_str);
                ptr_prev_str = ptr_str + 1;
            }
            ptr_str++;
        }

        if (*ptr_prev_str)
            arr.push_back(ptr_prev_str);

        for (int i = 0; i < arr.size(); ++i){
            if (pattern_match_body(str, arr[i], igrnoe_case))
                return true;
        }
        return false;
    }


     vector<string> find_files(const string& directory, const string& filter, bool findDirectory, bool includeSubDirectory)
    {
        string realpath = directory;
        if (realpath.empty())
            realpath = "./";

        char backchar = realpath.back();
        if (backchar != '\\' && backchar != '/')
            realpath += "/";

        struct dirent* fileinfo;
        DIR* handle;
        stack<string> ps;
        vector<string> out;
        ps.push(realpath);

        while (!ps.empty())
        {
            string search_path = ps.top();
            ps.pop();

            handle = opendir(search_path.c_str());
            if (handle != 0)
            {
                while (fileinfo = readdir(handle))
                {
                    struct stat file_stat;
                    if (strcmp(fileinfo->d_name, ".") == 0 || strcmp(fileinfo->d_name, "..") == 0)
                        continue;

                    if (lstat((search_path + fileinfo->d_name).c_str(), &file_stat) < 0)
                        continue;

                    if (!findDirectory && !S_ISDIR(file_stat.st_mode) ||
                        findDirectory && S_ISDIR(file_stat.st_mode))
                    {
                        if (pattern_match(fileinfo->d_name, filter.c_str()))
                            out.push_back(search_path + fileinfo->d_name);
                    }

                    if (includeSubDirectory && S_ISDIR(file_stat.st_mode))
                        ps.push(search_path + fileinfo->d_name + "/");
                }
                closedir(handle);
            }
        }
        return out;
    }

    string align_blank(const string& input, int align_size, char blank){
        if(input.size() >= align_size) return input;
        string output = input;
        for(int i = 0; i < align_size - input.size(); ++i)
            output.push_back(blank);
        return output;
    }


    bool save_file(const string& file, const void* data, size_t length, bool mk_dirs){

        if (mk_dirs){
            int p = (int)file.rfind('/');

            if (p != -1){
                if (!mkdirs(file.substr(0, p).c_str()))
                    return false;
            }
        }

        FILE* f = fopen(file.c_str(), "wb");
        if (!f) return false;

        if (data && length > 0){
            if (fwrite(data, 1, length, f) != length){
                fclose(f);
                return false;
            }
        }
        fclose(f);
        return true;
    }

    bool save_file(const string& file, const string& data, bool mk_dirs){
        return save_file(file, data.data(), data.size(), mk_dirs);
    }

    bool save_file(const string& file, const vector<uint8_t>& data, bool mk_dirs){
        return save_file(file, data.data(), data.size(), mk_dirs);
    }


    static volatile bool g_has_exit_signal = false;
    static int g_signum = 0;
    static void signal_callback_handler(int signum){
        INFO("Capture interrupt signal.");
        g_has_exit_signal = true;
        g_signum = signum;
    }

    int while_loop(){
        signal(SIGINT, signal_callback_handler);
        signal(SIGQUIT, signal_callback_handler);
        while(!g_has_exit_signal){
            this_thread::yield();
        }
        INFO("Loop over.");
        return g_signum;
    }


    string format(const char* fmt, ...) {
        va_list vl;
        va_start(vl, fmt);
        char buffer[2048];
        vsnprintf(buffer, sizeof(buffer), fmt, vl);
        return buffer;
    }


    const char* log_level(int level) {
        switch (level) {
        case LFATAL:    return "FATAL";
        case LERROR:    return "ERROR";
        case LWARN:     return "WARN";
        case LINFO:     return "INFO";
        case LVERBOSE:  return "VERBOSE";
        default:        return "UNKONWN";
        }
        return "UNKNOWN";
    }


    static struct Logger{
        mutex logger_lock_;
        string logger_directory;
        int logger_level{LINFO};
        vector<string> cache_, local_;
        shared_ptr<thread> flush_thread_;
        atomic<bool> keep_run_{false};
        shared_ptr<FILE> handler;
        bool logger_shutdown{false};

        void write(const string& line) {

            lock_guard<mutex> l(logger_lock_);
            if(logger_shutdown) 
                return;

            if (!keep_run_) {

                if(flush_thread_) 
                    return;

                cache_.reserve(1000);
                keep_run_ = true;
                flush_thread_.reset(new thread(std::bind(&Logger::flush_job, this)));
            }
            cache_.emplace_back(line);
        }

        void flush() {

            if (cache_.empty())
                return;

            {
                std::lock_guard<mutex> l(logger_lock_);
                std::swap(local_, cache_);
            }

            if (!local_.empty() && !logger_directory.empty()) {

                string now = date_now();
                string file = format("%s%s.txt", logger_directory.c_str(), now.c_str());
                if (!exists(file.c_str())) {
                    handler.reset(fopen_mkdirs(file.c_str(), "wb"), fclose);
                }
                else if (!handler) {
                    handler.reset(fopen_mkdirs(file.c_str(), "a+"), fclose);
                }

                if (handler) {
                    for (auto& line : local_)
                        fprintf(handler.get(), "%s\n", line.c_str());
                    fflush(handler.get());
                    handler.reset();
                }
            }
            local_.clear();
        }

        void flush_job() {

            auto tick_begin = timestamp_now();
            std::vector<string> local;
            while (keep_run_) {

                if (timestamp_now() - tick_begin < 1000) {
                    this_thread::sleep_for(std::chrono::milliseconds(100));
                    continue;
                }

                tick_begin = timestamp_now();
                flush();
            }
            flush();
        }

        void set_save_directory(const string& loggerDirectory) {
            logger_directory = loggerDirectory;

            if (logger_directory.empty())
                logger_directory = ".";


            if (logger_directory.back() != '/') {
                logger_directory.push_back('/');
            }

        }

        void set_logger_level(int level){
            logger_level = level;
        }

        void close(){
            {
                lock_guard<mutex> l(logger_lock_);
                if (logger_shutdown) return;

                logger_shutdown = true;
            };

            if (!keep_run_) return;
            keep_run_ = false;
            flush_thread_->join();
            flush_thread_.reset();
            handler.reset();
        }

        virtual ~Logger(){
            close();
        }
    }__g_logger;


    void set_logger_save_directory(const string& loggerDirectory){
        __g_logger.set_save_directory(loggerDirectory);
    }

    void set_log_level(int level){
        __g_logger.set_logger_level(level);
    }

    void __log(const string& file, int line, int level, const char* fmt, ...) {

        if(level > __g_logger.logger_level)
            return;

        string now = time_now();
        va_list vl;
        va_start(vl, fmt);
        
        char buffer[2048];
        string filename = file_name(file, true);
        int n = snprintf(buffer, sizeof(buffer), "[%s]", now.c_str());

        if (level == LFATAL || level == LERROR) {
            n += snprintf(buffer + n, sizeof(buffer) - n, "[\033[31m%s\033[0m]", log_level(level));
        }
        else if (level == LWARN) {
            n += snprintf(buffer + n, sizeof(buffer) - n, "[\033[33m%s\033[0m]", log_level(level));
        }
        else {
            n += snprintf(buffer + n, sizeof(buffer) - n, "[\033[32m%s\033[0m]", log_level(level));
        }


        n += snprintf(buffer + n, sizeof(buffer) - n, "[%s:%d]:", filename.c_str(), line);
        vsnprintf(buffer + n, sizeof(buffer) - n, fmt, vl);

        if (level == LFATAL || level == LERROR) {
            fprintf(stderr, "%s\n", buffer);
        }
        else if (level == LWARN) {
            fprintf(stdout, "%s\n", buffer);
        }
        else {
            fprintf(stdout, "%s\n", buffer);
        }

        if(!__g_logger.logger_directory.empty()){
            // remove save color txt
            // remove_color_text(buffer);
            __g_logger.write(buffer);
            if (level == LFATAL) {
                __g_logger.flush();
                fflush(stdout);
                abort();
            }
        }
    }




} // end namespace log