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
#include <map>
#include <functional>

namespace common
{
    class Logger;
    class LogEvent
    {
    public:
        typedef std::shared_ptr<LogEvent> ptr;
        LogEvent(const char* file, int32_t line, uint32_t elapse, 
            uint32_t thread_id, uint32_t fiber_id, uint64_t time);

        const char *getFile() const {
            return m_file;
        }

        int32_t getLine() const {
            return m_line;
        }

        uint32_t getElapse() const {
            return m_elapse;
        }

        uint32_t getThreadId() const {
            return m_threadId;
        }

        uint32_t getFiberId() const {
            return m_fiberId;
        }

        uint64_t getTime() const {
            return m_time;
        }

        const std::string getContent() const {
            return m_ss.str();
        }

        const std::stringstream& getSS() const {
            return m_ss;
        }

        const std::string &getName() const {
            return m_name;
        }

    private:
        const char* m_file = nullptr;   //文件名
        int32_t m_line = 0;             //行号
        uint32_t m_elapse = 0;          //程序启动开始到现在的毫秒数
        uint32_t m_threadId = 0;       //线程id
        uint32_t m_fiberId = 0;        //协程id
        uint64_t m_time;               //时间戳
        std::stringstream m_ss;         //内容
        std::string m_name;
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

        static const char* to_string(LogLevel::Level level);
    };

    //日志格式器
    class LogFormatter
    {
    public:
        typedef std::shared_ptr<LogFormatter> ptr;
        LogFormatter(const std::string& pattern);
        virtual ~LogFormatter();
        std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
        std::ostream& format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    public:
        class FormatItem {
        public:
            typedef std::shared_ptr<FormatItem> ptr;
            FormatItem(const std::string & fmt = "") {};
            virtual ~FormatItem(){}
            virtual void format(std::shared_ptr<Logger> logger, LogLevel::Level level, std::ostream& os, LogEvent::ptr event) = 0;
        };

        void init();

        bool isError() const { return m_error;}
    private:
        std::string m_pattern;
        std::vector<FormatItem::ptr> m_items;
        bool m_error = false;
    };

    //日志输出地
    class LogAppender
    {
    public:
        typedef std::shared_ptr<LogAppender> ptr;
        virtual ~LogAppender();
        //纯虚函数
        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
        void setFormatter(LogFormatter::ptr formatter) {m_formatter = formatter;}
        LogFormatter::ptr getLogFormatter() const {return m_formatter;}
        LogLevel::Level getLevel() const { return m_level;}
        void setLevel(LogLevel::Level val) { m_level = val;}
    protected:
        LogLevel::Level m_level = LogLevel::DEBUG;
        LogFormatter::ptr m_formatter;
    };


    //日志器
    class Logger : public std::enable_shared_from_this<Logger>
    {
    public:
        typedef std::shared_ptr<Logger> ptr;

        Logger(const std::string& name = "root");
        ~Logger();

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
        std::string getName() { return m_name;}
    private:
        std::string m_name;
        LogLevel::Level m_level;
        std::list<LogAppender::ptr> m_appenderList; //Appender集合
        LogFormatter::ptr m_formatter;
    };

    //输出到控制台的Appender
    class StdoutLogAppender : public LogAppender
    {
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;
        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
    private:
    };

    //输出到文件的Appender
    class FileLogAppender: public LogAppender
    {
    public:
        typedef std::shared_ptr<FileLogAppender> ptr;
        FileLogAppender(const std::string& filename);
        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;

        bool reopen();
    private:
        std::string m_filename;
        std::ofstream m_filestream;
    };
}


#endif //CPP_COMMON_LOG_H
