#include <stdio.h>
#include "log.h"

int main() {
    liux::Logger::ptr logger(new liux::Logger("test_logger"));
    logger -> addAppender(liux::LogAppender::ptr(new liux::StdoutLogAppender));

    liux::FileLogAppender::ptr file_appander( 
        new liux::FileLogAppender("./log/test.log"));
    liux::LogFormatter::ptr fmt(new liux::LogFormatter);
    file_appander->setFormatter(fmt);
    logger -> addAppender(file_appander);

    liux::LogEvent::ptr event(new liux::LogEvent("noname", 
                                            liux::LogLevel::ERROR, 
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