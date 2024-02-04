#include <iostream>
#include "../common/log.h"
using namespace std;
using namespace common;

int main()
{

    Logger::ptr logger(new Logger);
    LogAppender* appender = new StdoutLogAppender;
    appender->setLevel(LogLevel::DEBUG);
    logger->addAppender(LogAppender::ptr(appender));
    LogEvent::ptr event(new LogEvent(__FILE__, __LINE__, 0, 1, 2, time(0)));

    Logger* l = logger.get();
    l->log(LogLevel::INFO, event);

    cout << "hello" << endl;
    return EXIT_SUCCESS;
}