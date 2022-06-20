
#include "util.h"

#include <unistd.h>     // SYS_gettid 
#include <sys/syscall.h>  // syscall()
#include <algorithm>
#include <cstring>


namespace liux {

pid_t GetThreadId() {
    // 获取调用这个函数的线程的真实ID，而不是 POSIX 中 pthread_t 线程的可能存在重复的 id
    return syscall(SYS_gettid);    
}

// 需要协程库，暂不实现，随便返回一个值

uint64_t GetFiberId(){   
    return 123ul;
}


/*
struct timespec {
    time_t tv_sec;
    long tv_nsec;
};
*/
uint64_t GetElapsedMS() {
    struct timespec ts = {0};
    // CLOCK_MONOTONIC_RAW: 计算机系统重启到现在的时间
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1e6;
}

std::string GetThreadName() {
    char thread_name[16] = {0};
    pthread_getname_np(pthread_self(), thread_name, 16);
    return std::string(thread_name);
}

void SetThreadName(const std::string& name) {
    pthread_setname_np(pthread_self(), name.substr(0, 15).c_str());
}

/*
struct timeval {
    time_t tv_sec;
    long tv_usec;
};
*/
uint64_t GetCurrentMS() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    // 数字后面加 ul：该数值是 unsigned long 无符号整型
    return tv.tv_sec * 1000ul + tv.tv_usec / 1000;
}

uint64_t GetCurrentUS() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 * 1000ul + tv.tv_usec;
}

/**
template< class InputIt,
          class OutputIt,
          class UnaryOperation >
OutputIt transform( InputIt first1,
                    InputIt last1,
                    OutputIt d_first, 
                    UnaryOperation unary_op )
*/
std::string ToUpper(const std::string &name) {
    std::string rt = name;
    std::transform(rt.begin(), rt.end(), rt.begin(), ::toupper);
    return rt;
}

std::string ToLower(const std::string &name) {
    std::string rt = name;
    std::transform(rt.begin(), rt.end(), rt.begin(), ::tolower);
    return rt;
}

/**
struct tm {
    int    tm_sec;   // seconds [0,61]
    int    tm_min;   // minutes [0,59]
    int    tm_hour;  // hour [0,23]
    int    tm_mday;  // day of month [1,31]
    int    tm_mon;   // month of year [0,11]
    int    tm_year;  // years since 1900
    int    tm_wday;  // day of week [0,6] (Sunday = 0)
    int    tm_yday;  // day of year [0,365]
    int    tm_isdst; // daylight savings flag
}
*/
std::string Time2Str(time_t ts, const std::string &format) {
    struct tm tm;
    localtime_r(&ts, &tm);
    char buf[64];
    strftime(buf, sizeof(buf), format.c_str(), &tm);
    return buf;
}

time_t Str2Time(const char* str, const char* format) {
    struct tm t;
    std::memset(&t, 0, sizeof(t));
    if (!strptime(str, format, &t)) {
        return 0;
    }
    return mktime(&t);
}

} // end liux