#ifndef MIHOOKE_EXCEPTION_H
#define MIHOOKE_EXCEPTION_H

#include <exception>
#include <string>
#include "CurrentThread.h"

namespace mihooke
{

class Exception : public std::exception
{
public:
    Exception(const std::string &msg) : _msg(msg), _stack(stackTrace())
    {}

    const std::string& stack() const {return _stack;}
    const std::string& msg() const {return _msg;}
private:
    std::string _msg;
    std::string _stack;
};

} // namespace mihooke

#endif