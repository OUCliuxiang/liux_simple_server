#ifndef __NONCOPYABLE_H__
#define __NONCOPYABLE_H__

class noncopyable
{
public:
    noncopyable(const noncopyable&) = delete;
    void operator=(const noncopyable&) = delete;

protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

#endif // __NONCOPYABLE_H__