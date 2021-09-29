#ifndef MIHOOKE_ASYNC_LOGGING_H
#define MIHOOKE_ASYNC_LOGGING_H

#include <thread>
#include <memory>
#include <vector>

#include "LogStream.h"
#include "Mutex.h"
#include "Condition.h"
#include "CountDown.h"

namespace mihooke
{

class AsyncLogging
{
public:
    AsyncLogging(const std::string &filename);
    ~AsyncLogging();

    void start();
    void append(const char *msg, int len);
public:
    using Buffer = FixedBuffer<1000 * 4000>;
    using BufferVec = std::vector<std::unique_ptr<Buffer>>;
    using BufferPtr = std::unique_ptr<Buffer>;
private:
    void run();
private:
    bool _isRuning;
    std::string _filename;
    Mutex _mutex;
    Condition _cond;
    CountDown _countDown;
    std::unique_ptr<std::thread> _thread;
    BufferPtr _frontBuffer;
    BufferPtr _frontBufferBk;
    BufferVec _buffers;
};

} // namespace mihooke


#endif