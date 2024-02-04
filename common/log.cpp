#include "log.h"

namespace common
{
    const char* LogLevel::to_string(LogLevel::Level level)
    {
        switch (level) {
#define XX(name) \
        case LogLevel::name: \
            return #name;    \
            break;
            XX(DEBUG);
            XX(INFO);
            XX(WARN);
            XX(ERROR);
            XX(FATAL);
#undef XX
            default:
                return "UNKNOW";
        }
    }

    class MessageFormatItem : public LogFormatter::FormatItem
    {
    public:
        MessageFormatItem(const std::string & fmt = "") {};
        void format(Logger::ptr logger, LogLevel::Level level,
                    std::ostream& os, LogEvent::ptr event) override {
            os << event -> getContent();
        }
    };

    class LogLevelFormatItem : public LogFormatter::FormatItem
    {
    public:
        LogLevelFormatItem(const std::string & fmt = "") {};
        void format(Logger::ptr logger, LogLevel::Level level,
                    std::ostream& os, LogEvent::ptr event) override {
            os << LogLevel::to_string(level);
        }
    };

    class ElapseFormatItem : public LogFormatter::FormatItem
    {
    public:
        ElapseFormatItem(const std::string & fmt = "") {};
        void format(Logger::ptr logger, LogLevel::Level level,
                    std::ostream& os, LogEvent::ptr event) override {
            os << event -> getElapse();
        }
    };

    class NameFormatItem : public LogFormatter::FormatItem
    {
    public:
        NameFormatItem(const std::string & fmt = "") {};
        void format(Logger::ptr logger, LogLevel::Level level,
                    std::ostream& os, LogEvent::ptr event) override {
            os << event -> getName();
        }
    };

    class ThreadIdFormatItem : public LogFormatter::FormatItem
    {
    public:
        ThreadIdFormatItem(const std::string & fmt = "") {};
        void format(Logger::ptr logger, LogLevel::Level level,
                    std::ostream& os, LogEvent::ptr event) override {
            os << event -> getThreadId();
        }
    };

    class FiberIdFormatItem : public LogFormatter::FormatItem
    {
    public:
        FiberIdFormatItem(const std::string & fmt = "") {};
        void format(Logger::ptr logger, LogLevel::Level level,
                    std::ostream& os, LogEvent::ptr event) override {
            os << event -> getFiberId();
        }
    };

    class DateTimeIdFormatItem : public LogFormatter::FormatItem
    {
    public:
        DateTimeIdFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S") : m_format(format) {}
        void format(Logger::ptr logger, LogLevel::Level level,
                    std::ostream& os, LogEvent::ptr event) override {
            os << event -> getTime();
        }
    private:
        std::string m_format;
    };

    class FilenameFormatItem : public LogFormatter::FormatItem
    {
    public:
        FilenameFormatItem(const std::string & fmt = "") {};
        void format(Logger::ptr logger, LogLevel::Level level,
                    std::ostream& os, LogEvent::ptr event) override {
            os << event -> getFile();
        }
    };

    class LineFormatItem : public LogFormatter::FormatItem
    {
    public:
        LineFormatItem(const std::string & fmt = "") {};
        void format(Logger::ptr logger, LogLevel::Level level,
                    std::ostream& os, LogEvent::ptr event) override {
            os << event -> getLine();
        }
    };

    class NewLineFormatItem : public LogFormatter::FormatItem
    {
    public:
        NewLineFormatItem(const std::string & fmt = "") {};
        void format(Logger::ptr logger, LogLevel::Level level,
                    std::ostream& os, LogEvent::ptr event) override {
            os << std::endl;
        }
    };

    class StringFormatItem : public LogFormatter::FormatItem
    {
    public:
        StringFormatItem(const std::string& str) : LogFormatter::FormatItem(str), m_string(str) {}
        void format(Logger::ptr logger, LogLevel::Level level,
                    std::ostream& os, LogEvent::ptr event) override {
            os << m_string;
        }
    private:
        std::string m_string;
    };

    LogEvent::LogEvent(const char* file, int32_t line, uint32_t elapse, 
            uint32_t thread_id, uint32_t fiber_id, uint64_t time)
            :m_file(file), m_line(line),m_elapse(elapse), m_threadId(thread_id), m_fiberId(fiber_id), m_time(time)
    {

    }

    Logger::Logger(const std::string& name) : m_name(name), m_level(LogLevel::INFO)
    {
        m_formatter.reset(new LogFormatter("%d [%p] %f:%l %m %n"));
        //std::cout << m_name << std::endl;
    }

    Logger::~Logger() {

    }

    void Logger::addAppender(LogAppender::ptr appender)
    {
        if (!appender->getLogFormatter())
        {
            appender->setFormatter(m_formatter);
        }
        
        m_appenderList.push_back(appender);
    }

    void Logger::delAppender(LogAppender::ptr appender)
    {
        for (auto it = m_appenderList.begin(); it != m_appenderList.end(); it++) {
            if (*it == appender)
            {
                m_appenderList.erase(it);
                break;
            }
        }
    }

    void Logger::log(LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            for (auto& i: m_appenderList)
            {
                auto self = shared_from_this();
                i->log(self, level, event);
            }
        }
    }

    void Logger::debug(LogEvent::ptr event)
    {
        log(LogLevel::DEBUG, event);
    }
    void Logger::info(LogEvent::ptr event)
    {
        log(LogLevel::INFO, event);
    }
    void Logger::warn(LogEvent::ptr event)
    {
        log(LogLevel::WARN, event);
    }
    void Logger::error(LogEvent::ptr event)
    {
        log(LogLevel::ERROR, event);
    }
    void Logger::fatal(LogEvent::ptr event)
    {
        log(LogLevel::FATAL, event);
    }

    LogAppender::~LogAppender() {
        std::cout << "~LogAppender" << std::endl;
    }

    FileLogAppender::FileLogAppender(const std::string &filename) :m_filename(filename)
    {

    }

    void FileLogAppender::log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            m_formatter -> format(m_filestream, logger, level, event);
        }
    }

    bool FileLogAppender::reopen()
    {
        if (m_filestream)
        {
            m_filestream.close();
        }
        m_filestream.open(m_filename);
        //双感叹号，转成0或者1
        return !!m_filestream;
    }

    void StdoutLogAppender::log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            m_formatter->format(std::cout, logger, level, event);
        }
    }

    LogFormatter::LogFormatter(const std::string &pattern) : m_pattern(pattern){
        init();
    }

    std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
        std::stringstream ss;
        for(auto& i : m_items) {
            i->format(logger, level, ss, event);
        }
        return ss.str();
    }

    std::ostream& LogFormatter::format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
        for(auto& i : m_items) {
            i->format(logger, level, ofs, event);
        }
        return ofs;
    }

    LogFormatter::~LogFormatter() {
        std::cout << "~LogFormatter" << std::endl;
    }

    void LogFormatter::init() {
        //str, format, type
        std::vector<std::tuple<std::string, std::string, int> > vec;
        std::string nstr;
        for(size_t i = 0; i < m_pattern.size(); ++i) {
            if(m_pattern[i] != '%') {
                nstr.append(1, m_pattern[i]);
                continue;
            }

            if((i + 1) < m_pattern.size()) {
                if(m_pattern[i + 1] == '%') {
                    nstr.append(1, '%');
                    continue;
                }
            }

            size_t n = i + 1;
            int fmt_status = 0;
            size_t fmt_begin = 0;

            std::string str;
            std::string fmt;
            while(n < m_pattern.size()) {
                if(!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{'
                        && m_pattern[n] != '}')) {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    break;
                }
                if(fmt_status == 0) {
                    if(m_pattern[n] == '{') {
                        str = m_pattern.substr(i + 1, n - i - 1);
                        //std::cout << "*" << str << std::endl;
                        fmt_status = 1; //解析格式
                        fmt_begin = n;
                        ++n;
                        continue;
                    }
                } else if(fmt_status == 1) {
                    if(m_pattern[n] == '}') {
                        fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                        //std::cout << "#" << fmt << std::endl;
                        fmt_status = 0;
                        ++n;
                        break;
                    }
                }
                ++n;
                if(n == m_pattern.size()) {
                    if(str.empty()) {
                        str = m_pattern.substr(i + 1);
                    }
                }
            }

            if(fmt_status == 0) {
                if(!nstr.empty()) {
                    vec.push_back(std::make_tuple(nstr, std::string(), 0));
                    nstr.clear();
                }
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n - 1;
            } else if(fmt_status == 1) {
                std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
                m_error = true;
                vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            }
        }

        if(!nstr.empty()) {
            vec.push_back(std::make_tuple(nstr, "", 0));
        }

        static std::map<std::string, std::function<FormatItem::ptr(const std::string&)>> s_format_items = {
#define XX(str, C) \
                {#str, [](const std::string& fmt) {return FormatItem::ptr(new C(fmt));}}
                XX(m, MessageFormatItem),
                XX(p, LogLevelFormatItem),
                XX(r, ElapseFormatItem),
                XX(c, NameFormatItem),
                XX(t, ThreadIdFormatItem),
                XX(n, NameFormatItem),
                XX(d, DateTimeIdFormatItem),
                XX(f, FilenameFormatItem),
                XX(l, LineFormatItem)
#undef XX
        };
        for (auto& i: vec) {
            if (std::get<2>(i) == 0){
                m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
            }else{
                auto it = s_format_items.find(std::get<0>(i));
                if (it == s_format_items.end())
                {
                    m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                    m_error = true;
                }
                else
                {
                    m_items.push_back(it->second(std::get<1>(i)));
                }
            }
        }
    }

}