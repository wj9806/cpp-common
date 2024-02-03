#include "log.h"

namespace common
{
    Logger::Logger(const std::string& name) : m_name(name)
    {

    }

    void Logger::addAppender(LogAppender::ptr appender)
    {
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
                i->log(level, event);
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

    FileLogAppender::FileLogAppender(const std::string &filename) :m_filename(filename)
    {

    }

    void FileLogAppender::log(LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            m_filestream << m_formatter -> format(event);
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

    void StdoutLogAppender::log(LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            std::cout << m_formatter->format(event);
        }
    }

    LogFormatter::LogFormatter(const std::string &pattern) : m_pattern(pattern){
    }

    std::string LogFormatter::format(std::ostream& os, LogEvent::ptr event) {
        std::stringstream ss;
        for (const auto &item: m_items)
        {
            item->format(event);
        }
        return ss.str();
    }

    void LogFormatter::init() {
        //string format type
        std::vector<std::tuple<std::string, std::string, int>> vec;
        std::string nstr;
        for(size_t i = 0; i < m_pattern.size(); i ++)
        {
            if (m_pattern[i] != '%')
            {
                nstr.append(1, m_pattern[i]);
                continue;
            }
            if ((i + 1) < m_pattern.size() && m_pattern[i + 1] == '%')
            {
                nstr.append(1, '%');
                continue;
            }
            size_t n = i+1;
            size_t fmt_status = 0;
            size_t fmt_begin = 0;
            std::string str;
            std::string fmt;
            while (n < m_pattern.size())
            {
                if (isspace(m_pattern[1]))
                    break;
                if (fmt_status == 0)
                {
                    if (m_pattern[i] == '{')
                    {
                        str = m_pattern.substr(i + 1, n - 1 - 1);
                        fmt_status = 1;
                        fmt_begin = n;
                        n++;
                        continue;
                    }
                }
                if (fmt_status == 1)
                {
                    if (m_pattern[n] == '}')
                    {
                        fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin -1);
                        fmt_status = 2;
                        break;
                    }
                }
            }
            if (fmt_status == 0)
            {
                if (!nstr.empty())
                {
                    vec.push_back(std::make_tuple(nstr, "", 0));
                }
                str = m_pattern.substr(i +1, n -i - 1);
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n;
            } else if (fmt_status == 1)
            {
                std::cout << "pattern parse error: "
                    << m_pattern << "-" << m_pattern.substr(i)
                    << std::endl;
                vec.push_back(std::make_tuple("<<pattern error>>", fmt, 0));
            } else if(fmt_status == 2)
            {
                if (!nstr.empty())
                {
                    vec.push_back(std::make_tuple(nstr, "", 0));
                }
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n;
            }
        }

        if (!nstr.empty())
        {
            vec.push_back(std::make_tuple(nstr, "", 0));
        }
    }
}
