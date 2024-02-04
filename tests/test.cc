#include <iostream>
#include "../common/log.h"
using namespace std;
using namespace common;

int main()
{
    Logger::ptr logger(new Logger);
    logger->addAppender(LogAppender::ptr(new StdoutLogAppender));
    LogEvent::ptr event(new LogEvent(__FILE__, __LINE__, 0, 1, 2, time(0)));

    logger->log(LogLevel::INFO, event);

    cout << "hello" << endl;
    return EXIT_SUCCESS;
}