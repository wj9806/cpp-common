#ifndef CPP_COMMON_LOG_H
#define CPP_COMMON_LOG_H

#include <string>
#include <cstdint>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>

namespace common
{
    class LogEvent
    {
    public:
        typedef std::shared_ptr<LogEvent> ptr;
        LogEvent();
    private:
        const char* m_file = nullptr;   //文件名
        int32_t m_line = 0;             //行号
        uint32_t m_elapse = 0;          //程序启动开始到现在的毫秒数
        uint32_t m_threadId = 0;       //线程id
        uint32_t m_fiberId = 0;        //协程id
        uint64_t m_time;               //时间戳
        std::string m_content;         //内容
    };

    class LogLevel
    {
    public:
        enum Level {
            DEBUG = 1,
            INFO = 2,
            WARN = 3,
            ERROR = 4,
            FATAL = 5
        };
    };

    //日志格式器
    class LogFormatter
    {
    public:
        typedef std::shared_ptr<LogFormatter> ptr;
        LogFormatter(const std::string& pattern);
        virtual std::string format(std::ostream& os, LogEvent::ptr event);
    private:
        class FormatItem {
        public:
            typedef std::shared_ptr<FormatItem> ptr;
            virtual ~FormatItem(){}
            virtual std::string format(LogEvent::ptr event) = 0;
        };

        void init();
    private:
        std::string m_pattern;
        std::vector<FormatItem::ptr> m_items;
    };

    //日志输出地
    class LogAppender
    {
    public:
        typedef std::shared_ptr<LogAppender> ptr;
        virtual ~LogAppender();
        //纯虚函数
        virtual void log(LogLevel::Level level, LogEvent::ptr event) = 0;
        void setFormatter(LogFormatter::ptr formatter) {m_formatter = formatter;}
        LogFormatter::ptr getLogFormatter() const {return m_formatter;}
    protected:
        LogLevel::Level m_level;
        LogFormatter::ptr m_formatter;
    };


    //日志器
    class Logger
    {
    public:
        typedef std::shared_ptr<Logger> ptr;

        Logger(const std::string& name = "root");

        void log(LogLevel::Level level, LogEvent::ptr event);

        void debug(LogEvent::ptr event);
        void info(LogEvent::ptr event);
        void warn(LogEvent::ptr event);
        void error(LogEvent::ptr event);
        void fatal(LogEvent::ptr event);

        void addAppender(LogAppender::ptr appender);
        void delAppender(LogAppender::ptr appender);
        LogLevel::Level getLevel() const {return m_level;}
        void setLevel(LogLevel::Level level) {m_level = level;}
    private:
        std::string m_name;
        LogLevel::Level m_level;
        std::list<LogAppender::ptr> m_appenderList; //Appender集合
    };

    //输出到控制台的Appender
    class StdoutLogAppender : public LogAppender
    {
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;
        virtual void log(LogLevel::Level level, LogEvent::ptr event) override;
    private:
    };

    //输出到文件的Appender
    class FileLogAppender: public LogAppender
    {
    public:
        typedef std::shared_ptr<FileLogAppender> ptr;
        FileLogAppender(const std::string& filename);
        virtual void log(LogLevel::Level level, LogEvent::ptr event) override;

        bool reopen();
    private:
        std::string m_filename;
        std::ofstream m_filestream;
    };
}


#endif //CPP_COMMON_LOG_H
