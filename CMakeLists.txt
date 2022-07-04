cmake_minimum_required(VERSION 2.8)
# set(CMAKE_BUILD_TYPE RelWithDebInfo)

project(liux) # 项目名称为 liux，存储在变量 PROJECT_NAME 中

set(CMAKE_VERBOSE_MAKEFILE ON) 
# 编译参数 -g 允许 gdb 调试模式
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O0 -std=c++11 -Wall -Werror")
# -rdynamic: 将所有符号都加入到符号表中，便于使用dlopen或者backtrace追踪到符号
# -fPIC: 生成位置无关的代码，便于动态链接
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -fPIC")
# -Wno-unused-function: 不要警告未使用函数
# -Wno-builtin-macro-redefined: 不要警告内置宏重定义，用于重定义内置的__FILE__宏
# -Wno-deprecated: 不要警告过时的特性
# -Wno-deprecated-declarations: 不要警告使用带deprecated属性的变量，类型，函数
# -fpermissive: 不加这个，boost 会报 assert 相关错误，依照编译器建议，添加该项
set(CMAKE_CXX_FLAGS "-Wno-deprecated -Wno-unused-function -fpermissive") 

# 本项目头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 寻找 Boost 头文件路径并加入项目
find_package(Boost REQUIRED) 
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

# 可执行文件生成到项目路径下的 bin，库文件生成到 lib。项目路径缺省就是当前路径 ./ 。
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 库文件的源文件
# set(LIB_SRC 
#     src/log.cpp
#     src/config.cpp
#     src/util.cpp
#     src/mutex.cpp
#     src/thread.cpp)
# add_library(liux SHARED ${LIB_SRC})    # SHARED 方式生成 liux.so 动态库

# 生成动态链接库
add_library(liux_log SHARED src/log.cpp)
add_library(liux_util SHARED src/util.cpp)
add_library(liux_mutex SHARED src/mutex.cpp)
add_library(liux_config SHARED src/config.cpp src/log.cpp src/mutex.cpp src/util.cpp )
add_library(liux_thread SHARED src/thread.cpp src/mutex.cpp)
add_library(liux_fiber SHARED src/fiber.cpp src/thread.cpp src/config.cpp src/log.cpp src/util.cpp )
add_library(liux_scheduler SHARED include/scheduler.h src/fiber.cpp src/log.cpp src/thread.cpp src/mutex.cpp)

set(LIBS 
    liux_log
    liux_config
    liux_util
    liux_mutex
    liux_thread
    liux_fiber
    # liux_liux_scheduler
    pthread
    yaml-cpp)

add_executable(test_log tests/test_log.cpp)      # 生成 test 测试文件 可执行文件
target_link_libraries(test_log ${LIBS})       # 将可执行文件 test_log 和头文件库文件连接起来    

add_executable(test_thread tests/test_thread.cpp)      # 生成 test 测试文件 可执行文件
target_link_libraries(test_thread ${LIBS})       # 将可执行文件 test_thread 和头文件库文件连接起来    


