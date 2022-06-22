// util 功能函数文件

#ifndef _UTIL_H__
#define _UTIL_H__

// 存储各种 typedef 重定义的基本类型，如 pid_t, time_t, size_t 等等，便于移植
#include <sys/types.h> 
#include <stdint.h> // typedef 重定义的整型类型，uint8_t, INT_MAX 等等
#include <sys/time.h>
#include <cxxabi.h> // abi::__cxa_demangle()   
#include <string>
#include <vector>
#include <iostream>

namespace liux {

// 获取线程 ID
pid_t GetThreadId();

// 获取协程 ID
uint64_t GetFiberId();

// 获取启动至今毫秒数
uint64_t GetElapsedMS();

// 获取线程名
std::string GetThreadName();

// 设置线程名
void SetThreadName(const std::string& name);

// 获取当前毫秒
uint64_t GetCurrentMS();

// 获取当前微妙
uint64_t GetCurrentUS();

// 字符串转为大写
std::string ToUpper(const std::string& name);

// 字符串转为小写
std::string Tolower(const std::string& name);

// 时间类型转为字符串
std::string Time2Str(time_t ts = time(NULL), const std::string& format= "%Y-%m-%d %H:%M:%S");

// 字符串转为时间类型
time_t Str2Time(const char* str, const char* format = "%Y-%m-%d %H:%M:%S");


// 文件系统操作类
class FSUtil {
public:
    // 递归列举指定目录下所有指定后缀的常规文件，如果不指定后缀，则遍历所有文件，返回的文件名带路径
    // param[out] files 返回的文件列表
    // param[in] path 路径
    // param[in] subfix 后缀
    static void ListAllFile(std::vector<std::string>& files, const std::string& path, const std::string& subfix);

    // 递归创建路径，相当于 mkdir -p ，返回是否成功
    static bool Mkdir(const std::string& dirpath);

    // 判断文件指定的 pid 是否正在运行
    static bool IsRunningPidFile(const std::string& pidfile);

    // 删除文件或路径，返回是否成功
    static bool Rm(const std::string& path);

    // 移动文件或目录，返回是否成功
    static bool Mv(const std::string& from, const std::string& to);

    // 获取文件绝对路径，路径中的符号链接会被解析成实际路径，删除多余的 '.' '..' '/'，返回是否成功
    static bool RealPath(const std::string& path, const std::string& rpath);

    // 建立软连接（符号链接），返回是否成功
    static bool Symlink(const std::string& from, const std::string& to);
    
    // 解除软连接，返回是否成功
    static bool Unlink(const std::string& filename, bool exist = false);

    // 获取文件的路径部分，也即最后一个 '/' 前面的部分，需要传入完整路径
    static std::string Dirname(const std::string& filename);

    // 获取文件的名字部分，也即最后一个 '/' 后面的部分，需要传入完整路径
    static std::string Basename(const std::string& filename);

    // 以流方式只读打开文件
    static bool OpenForRead(std::ifstream &ifs, const std::string& filename, std::ios_base::openmode mode);

    // 以流方式只写打开文件
    static bool OpenForWrite(std::ofstream &ifs, const std::string& filename, std::ios_base::openmode mode);

}; // end FSUtil


class TypeUtil {

public:
    // 字符串转字符，返回 *(str.begin())
    static int8_t ToChar(const std::string& str);

    // stoi
    static int64_t Atoi(const std::string& str);

    // stof
    static double Atof(const std::string& str);
    
    // 字符数组转字符，返回 str[0]
    static int8_t ToChar(const char* str);
    
    // atoi
    static int64_t Atoi(const char* str);

    // atof
    static double Atof(const char* str);

}; // end TypeUtil

template<class T> 
const char* TypeToName() {
    // abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr) 
    // from <cxxabi> ，返回类型 T 的完成名称。 
    static const char* s_name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
    return s_name;
};


} // end liux namespace

#endif // __UTIL_H__