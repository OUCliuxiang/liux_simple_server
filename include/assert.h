
/**          
 * 封装断言宏。断言：判断一个表达式，如果结果为假，输出诊断消息并中止程序。
 * ----------------------------------------------------------------
 * 使用到 gunc (gcc/g++ 编译器) 和 llvm 特有而 clang 不支持的选择优化特性，
 * __builtin_expect(!!(x), 1) 表示预期条件成立的可能性更大
 * __builtin_expect(!!(x), 0) 表示预期条件不成立的可能性更大
 * !!x 是取变量 x 的布尔值
 * 如果是 clang 编译器，则要把下面的宏避开，这里我们通过 #ifdef/#else 一并实现了吧
 * ---------------------------------------------------------------
 * 有点鸡肋，实际项目中未使用
*/

#ifndef __ASSERT_H__
#define __ASSERT_H__

#include <string>
#include <cassert>
#include "log.h"
#include "util.h"

#if defined __GUNC__ || defined __llvm__ // 如果使用 gcc/g++ 或者 llvm
// 告诉编译器 条件成立的
#define LIUX_LIKELY(x) __builtin_expect(!!(x), 1);
#define LIUX_UNLIKELY(x) __builtin_expect(!!(x), 0);
#else // clang 编译器，没有选择分支优化特性
#define LIUX_LIKELY(x) (x);
#define LIUX_UNLIKELY(x) (x);
#endif

// 断言宏
#define LIUX_ASSERT(x) if (LIUX_UNLIKELY(!(x))) { assert(x); }
#endif // __ASSERT_H__