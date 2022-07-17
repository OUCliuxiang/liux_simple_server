#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include <exception>
#include <string>
#include <string.h>
#include <errno.h>
#include <utility>

#define THROW_EXCEPTION_WHIT_ERRNO                       \
    do                                                   \
    {                                                    \
        throw Exception(std::string(::strerror(errno))); \
    } while (0)

/**
 * @brief std::exception 的封装
 * 增加了调用栈信息的获取接口
*/
class Exception : public std::exception
{
public:
    explicit Exception(std::string what): 
        m_message(std::move(what)), m_stack(std::to_string(200)) {
    }

    ~Exception() noexcept override = default;

    // 获取异常信息
    inline const char* what() const noexcept override {
        return m_message.c_str();
    }
    // 获取函数调用栈
    inline const char* stackTrace() const noexcept {
        return m_stack.c_str();
    }

protected:
    std::string m_message;
    std::string m_stack;
};

class SystemError : public Exception
{
public:
    explicit SystemError(std::string what = ""): 
    Exception(what + " : " + std::string(::strerror(errno)));
}


#endif // __EXCEPTION_H__
