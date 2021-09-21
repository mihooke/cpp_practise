#include "LogStream.h"

#include <algorithm>

namespace mihooke
{
namespace
{
    
const char digits[] = "9876543210123456789";
const char* zero = digits + 9;
static_assert(sizeof(digits) == 20, "wrong number of digits");

const char digitsHex[] = "0123456789ABCDEF";
static_assert(sizeof digitsHex == 17, "wrong number of digitsHex");

// Efficient Integer to String Conversions, by Matthew Wilson.
template<typename T>
size_t convert(char buf[], T value)
{
    T i = value;
    char* p = buf;

    do{
        int lsd = static_cast<int>(i % 10);
        i /= 10;
        *p++ = zero[lsd];
    } while (i != 0);

    if (value < 0)
    {
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

size_t convertHex(char buf[], uintptr_t value)
{
    uintptr_t i = value;
    char* p = buf;

    do{
        int lsd = static_cast<int>(i % 16);
        i /= 16;
        *p++ = digitsHex[lsd];
    } while (i != 0);

    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

}
template<typename T>
void LogStream::formatInteger(T v)
{
    if (_buf.rest() >= 32)
    {
        size_t len = convert(_buf.current(), v);
        _buf.addLength(len);
    }
}

LogStream &LogStream::operator<<(bool v)
{
    *this << (v ? 1 : 0);
    return *this;
}
LogStream &LogStream::operator<<(char v)
{
    append(&v, 1);
    return *this;
}
LogStream &LogStream::operator<<(short v)
{
    *this << static_cast<int>(v);
    return *this;
}
LogStream &LogStream::operator<<(unsigned short v)
{
    *this << static_cast<unsigned int>(v);
    return *this;
}
LogStream &LogStream::operator<<(int v)
{
    formatInteger(v);
    return *this;
}
LogStream &LogStream::operator<<(unsigned int v)
{
    formatInteger(v);
    return *this;
}
LogStream &LogStream::operator<<(long v)
{
    formatInteger(v);
    return *this;
}
LogStream &LogStream::operator<<(unsigned long v)
{
    formatInteger(v);
    return *this;
}
LogStream &LogStream::operator<<(long long v)
{
    formatInteger(v);
    return *this;
}
LogStream &LogStream::operator<<(unsigned long long v)
{
    formatInteger(v);
    return *this;
}
LogStream &LogStream::operator<<(float v)
{
    *this << static_cast<double>(v);
    return *this;
}
LogStream &LogStream::operator<<(double v)
{
    if (_buf.rest()>= 32)
    {
        int len = snprintf(_buf.current(), 32, "%.12g", v);
        _buf.addLength(len);
    }
    return *this;
}
LogStream &LogStream::operator<<(const void* v)
{
    uintptr_t p = reinterpret_cast<uintptr_t>(v);
    if (_buf.rest() >= 32)
    {
        char *buf = _buf.current();
        buf[0] = '0';
        buf[1] = 'x';
        size_t len = convertHex(buf+2, p);
        _buf.addLength(len);
    }
    return *this;
}
LogStream &LogStream::operator<<(const char* v)
{
    if (v == nullptr)
    {
        append("(null)", 6);
    }
    else
    {
        append(v, strlen(v));
    }
    return *this;
}
LogStream &LogStream::operator<<(const std::string& v)
{
    append(v.c_str(), v.size());
    return *this;
}

} // namespace mihooke

