#ifndef MIHOOKE_LOGGING_H
#define MIHOOKE_LOGGING_H

#include <string>
#include "LogStream.h"

namespace mihooke
{

enum LogLevel
{
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL,
    NUM_LOG_LEVELS,
};

class Logging
{
public:
    Logging(const std::string &filename, int line, LogLevel level, const std::string &funcname);
    ~Logging();

    LogStream& stream() {return _stream;}

    static LogLevel logLevel();
    static void setLogLevel(LogLevel level);

    using OutputFunc=void (*)(const char *msg, int len);
    using FlushFunc=void (*)();
    static void setOutput(OutputFunc);
    static void setFlush(FlushFunc);
private:
    LogStream _stream;
    std::string _filename;
    int _line;
    LogLevel _level;
    std::string _funcname;
};

extern LogLevel g_logLevel;

inline LogLevel Logging::logLevel()
{
    return g_logLevel;
}

#define LOG_TRACE if (mihooke::Logging::logLevel() <= mihooke::LogLevel::TRACE) \
    mihooke::Logging(__FILE__, __LINE__, mihooke::LogLevel::TRACE, __func__).stream()
#define LOG_DEBUG if (mihooke::Logging::logLevel() <= mihooke::LogLevel::DEBUG) \
    mihooke::Logging(__FILE__, __LINE__, mihooke::LogLevel::DEBUG, __func__).stream()
#define LOG_INFO if (mihooke::Logging::logLevel() <= mihooke::LogLevel::INFO)\
    mihooke::Logging(__FILE__, __LINE__, mihooke::LogLevel::INFO, __func__).stream()
#define LOG_WARNING if (mihooke::Logging::logLevel() <= mihooke::LogLevel::WARNING) \
    mihooke::Logging(__FILE__, __LINE__, mihooke::LogLevel::WARNING, __func__).stream()
#define LOG_ERROR if (mihooke::Logging::logLevel() <= mihooke::LogLevel::ERROR) \
    mihooke::Logging(__FILE__, __LINE__, mihooke::LogLevel::ERROR, __func__).stream()
#define LOG_FATAL if (mihooke::Logging::logLevel() <= mihooke::LogLevel::FATAL) \
    mihooke::Logging(__FILE__, __LINE__, mihooke::LogLevel::FATAL, __func__).stream()

}; // namespace mihooke

#endif