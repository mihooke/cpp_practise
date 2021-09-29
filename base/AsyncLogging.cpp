#include "AsyncLogging.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

namespace
{

void fwriten(FILE* file, const char *msg, int len)
{
    size_t remain = len;
    while (remain > 0)
    {
        size_t wn = fwrite(msg, 1, remain, file);
        if (wn == 0)
        {
            int err = ferror(file);
            if (err)
            {
                fprintf(stderr, "Write to file failed: %s\n", strerror(err));
            }
            break;
        }
        remain -= wn;
    }
}

} // namespace


namespace mihooke
{

AsyncLogging::AsyncLogging(const std::string &filename) : 
    _isRuning(true), _filename(filename), _mutex(), _cond(_mutex), _countDown(1),
    _frontBuffer(new Buffer), _frontBufferBk(new Buffer)
{
    _frontBuffer->bZero();
    _frontBufferBk->bZero();
    _buffers.reserve(16);
}

AsyncLogging::~AsyncLogging()
{
    if (_isRuning)
    {
        _isRuning = false;
        _cond.notify();
        _thread->join();
    }
}

void AsyncLogging::start()
{
    _isRuning = true;
    _thread = std::make_unique<std::thread>(&AsyncLogging::run, this);
    _countDown.wait();
}

void AsyncLogging::append(const char *msg, int len)
{
    MutexLockGuard lg(_mutex);
    if (_frontBuffer->rest() > len)
    {
        _frontBuffer->append(msg, len);
    }
    else
    {
        _buffers.push_back(std::move(_frontBuffer));
        if (_frontBufferBk)
        {
            _frontBuffer = std::move(_frontBufferBk);
        }
        else
        {
            _frontBuffer.reset(new Buffer);
        }
        _frontBuffer->append(msg, len);
        _cond.notify();
    }
}

void AsyncLogging::run()
{
    BufferPtr buffer1{new Buffer};
    BufferPtr buffer2{new Buffer};
    buffer1->bZero();
    buffer2->bZero();
    BufferVec toWrite;
    toWrite.reserve(16);
    FILE *file;
    file = fopen(_filename.c_str(), "ae");
    _countDown.down();
    while (_isRuning)
    {
        {
            MutexLockGuard lg(_mutex);
            if (_buffers.size() == 0)
            {
                _cond.wait(3);
            }
            _buffers.push_back(std::move(_frontBuffer));
            _frontBuffer = std::move(buffer1);
            toWrite.swap(_buffers);
            if (!_frontBufferBk)
            {
                _frontBufferBk = std::move(buffer2);
            }
        }
        for (const auto &buf : toWrite)
        {
            fwriten(file, buf->data(), buf->length());
        }

        if (toWrite.size() > 2)
        {
            toWrite.resize(2);
        }

        if (!buffer1)
        {
            buffer1 = std::move(toWrite.back());
            toWrite.pop_back();
            buffer1->clear();
        }

        if (!buffer2)
        {
            buffer2 = std::move(toWrite.back());
            toWrite.pop_back();
            buffer2->clear();
        }
        toWrite.clear();
        fflush(file);
    }
    fflush(file);
}

} // namespace mihooke

