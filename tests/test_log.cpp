#include <stdio.h>
#include "log.h"

int main() {
    sylar::Logger::ptr logger(new sylar::Logger("test_logger"));
    logger -> addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender));

    sylar::FileLogAppender::ptr file_appander( 
        new sylar::FileLogAppender("./log/test.log"));
    sylar::LogFormatter::ptr fmt(new sylar::LogFormatter);
    file_appander->setFormatter(fmt);
    logger -> addAppender(file_appander);

    sylar::LogEvent::ptr event(new sylar::LogEvent("noname", 
                                            sylar::LogLevel::ERROR, 
                                            "./log/test.log", 
                                            32,
                                            65535,
                                            22,
                                            33,
                                            334567244,
                                            "threadName"));
    event -> setLoggerName(logger -> getName());
    logger -> log(event);

    return 0;
    
}