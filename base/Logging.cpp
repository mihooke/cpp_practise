#include "Logging.h"
#include <sys/timeb.h>

namespace mihooke {

namespace{
void defaultOutput(const char *msg, int len)
{
    fwrite(msg, 1, len, stdout);
}

void defaultFlush()
{
    fflush(stdout);
}

const char *logLevelName(LogLevel level)
{
    static const char *names[LogLevel::NUM_LOG_LEVELS] = {
        "TRACE ",
        "DEBUG ",
        "INFO  ",
        "WARNING  ",
        "ERROR ",
        "FATAL ",};
    return names[level];
}

std::string now()
{
    /* c++11 only second
    auto dt{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};
    auto dtStr{std::put_time(std::localtime(&dt), "%Y-%m-%D %H:%M:%S")}
    */
    struct timeb tb;
    ftime(&tb);
    struct tm tm_loc;
    localtime_r(&tb.time, &tm_loc);
    char timestamp[25]{0};
    std::snprintf(timestamp, 25, "%4d-%02d-%02d %02d:%02d:%02d.%03d",
                  tm_loc.tm_year + 1900, tm_loc.tm_mon + 1, tm_loc.tm_mday, 
                  tm_loc.tm_hour, tm_loc.tm_min, tm_loc.tm_sec, tb.millitm);

    return timestamp;
}
}

Logging::OutputFunc g_output = defaultOutput;
Logging::FlushFunc g_flush = defaultFlush;

// 08:48:17.067 - INFO - adapter.cpp - 352 - func - message.
Logging::Logging(const std::string &filename, int line, LogLevel level, const std::string &funcname)
    : _filename(filename), _line(line), _level(level), _funcname(funcname)
    {
        _stream << now() << '-' << logLevelName(level) << '-' << _filename << '-' << _line << '-' << _funcname << '-';
    }
Logging::~Logging()
{
    _stream << '\n';
    const LogStream::Buffer& buf = _stream.buffer();
    g_output(buf.data(), buf.length());
    if (_level == LogLevel::FATAL)
    {
        g_flush();
        abort();
    }
}

LogLevel Logging::logLevel()
{
    return g_logLevel;
}
void Logging::setLogLevel(LogLevel level)
{
    g_logLevel = level;
}

void Logging::setOutput(OutputFunc func)
{
    g_output = func;
}

void Logging::setFlush(FlushFunc func)
{
    g_flush = func;
}

}