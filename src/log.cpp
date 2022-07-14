#include "log.h"
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <string>
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
#include <stdlib.h>
// #include <boost/legical_cast>


// 获取时间 tm 类型结构体的宏
// localtime 得到的是当地时区时间，gmtime 是格林尼治时间
#define __GetTimeBlock \
    time_t timep; \
    time(&timep); \
    tm& t = *(tm*) localtime(&timep);


namespace log {

    using namespace std;

    const char* level_string(int level) {
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
    
    const char* date_now() {
        // yyyy-mm-dd
        char time_string[20];
        __GetTimeBlock;
        // yyyy-mm-dd
        sprintf(time_string, "%04d-%02d-%02d", 
                t.tm_year+1970, t.tm_mon+1, t.tm_mday);
        return time_string;
    }

    const char* time_now() {
        // yyyy-mm-dd HH:MM:SS
        char time_string[20];
        __GetTimeBlock;
        sprintf(time_string, "%04d-%02d-%02d %02d:%02d:02d", 
                t.tm_year+1900, t.tm_mon+1, t.tm_mday, 
                t.tm_hour,      t.tm_min,   t.tm_sec);
        return time_string;
    }

    // 格林尼治格式时间，time.h 库有 gmtime 调用，这里再做一层封装
    const char* gmtime(time_t t) {
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

    const char* gmtime_now() {
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
        return  chrono::duration_cast<chrono::milliseconds>(\
                chrono::system_clock::now().time_since_epoch.count());
    }
    

    bool isfile(const char* file) {
        static stat st; // 描述文件属性的结构体 
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
        
        string directory = _path.substr(0, p);
        if (!mkdirs(directory))
            return nullptr;

        return fopen(path, mode);
    }

    
} // end namespace log