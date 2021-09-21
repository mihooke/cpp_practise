#ifndef MIHOOKE_LOGSTREAM_H
#define MIHOOKE_LOGSTREAM_H

#include "string.h"
#include <string>

namespace mihooke
{

template<int SIZE>
class FixedBuffer
{
public:
    FixedBuffer() : _cur(_buf) {}
    ~FixedBuffer() {}

    int length() const {return static_cast<int>(_cur - _buf);}
    int rest() const {return static_cast<int>(end() - _cur);}
    const char* end() const {return _buf + sizeof(_buf);}
    char* current() {return _cur;}
    const char* data() const {return _buf;}
    void bZero() {memset(_buf, 0, sizeof(char) * SIZE);}
    void clear() {_cur = _buf;}

    void addLength(size_t len)
    {
        _cur += (len+1);
        _buf[length()-1] = ' ';
    }
    void append(const char *msg, int len)
    {
        if (rest() > len)
        {
            memcpy(_cur, msg, len);
            addLength(len);
        }
    }
private:
    char _buf[SIZE];
    char *_cur;
};

class LogStream
{
public:
    static const int kSmallSize{4000};
    using Buffer = FixedBuffer<kSmallSize>;
public:
    LogStream() {}
    ~LogStream() {}

    void append(const char *msg, int len) {_buf.append(msg, len);}
    const Buffer& buffer() const {return _buf;}

    LogStream &operator<<(bool v);
    LogStream &operator<<(char v);
    LogStream &operator<<(short v);
    LogStream &operator<<(unsigned short v);
    LogStream &operator<<(int v);
    LogStream &operator<<(unsigned int v);
    LogStream &operator<<(long v);
    LogStream &operator<<(unsigned long v);
    LogStream &operator<<(long long v);
    LogStream &operator<<(unsigned long long v);
    LogStream &operator<<(float v);
    LogStream &operator<<(double v);
    LogStream &operator<<(const void* v);
    LogStream &operator<<(const char* v);
    LogStream &operator<<(const std::string& v);

private:
    template<typename T>
    void formatInteger(T);
private:
    Buffer _buf;
};
    
} // namespace mihooke

#endif