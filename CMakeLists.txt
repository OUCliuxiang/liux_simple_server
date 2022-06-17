cmake_minimum_required(VERSION 2.8)
# set(CMAKE_BUILD_TYPE RelWithDebInfo)

project(liux) # 项目名称为 liux，存储在变量 PROJECT_NAME 中

set(CMAKE_VERBOSE_MAKEFILE ON) 
# 编译参数
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function") 

include_directories(${PROJECT_SOURCE_DIR}/include)

# 库文件的源文件
set(LIB_SRC 
    src/log.cpp
    src/config.cpp
    src/util.cpp
    src/mutex.cpp)
add_library(liux SHARED ${LIB_SRC})    # SHARED 方式生成 liux.so 动态库

# add_executable(test_log tests/test_log.cpp)      # 生成 test 测试文件 可执行文件
# target_link_libraries(test_log liux)       # 将可执行文件 test_log 和头文件库文件连接起来    

# 可执行文件生成到项目路径下的 bin，库文件生成到 lib。项目路径缺省就是当前路径 ./ 。
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
