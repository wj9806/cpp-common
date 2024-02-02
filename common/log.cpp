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
        std::vector<std::pair<std::string, int>> vec;
        //unsigned int
        size_t last_pos = 0;

    }
}
