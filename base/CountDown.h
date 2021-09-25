#ifndef MIHOOKE_COUNTDOWN_H
#define MIHOOKE_COUNTDOWN_H

#include "Condition.h"

namespace mihooke
{

class CountDown
{
public:
    explicit CountDown(int count) : _mutex(), _cond(_mutex), _count(count)
    {
    }

    void down()
    {
        MutexLockGuard lg(_mutex);
        --_count;
        if (_count == 0)
        {
            _cond.notifyAll();
        }
    }

    void wait()
    {
        MutexLockGuard lg(_mutex);
        while (_count > 0)
        {
            _cond.wait();
        }
    }

    int count() const 
    {
        MutexLockGuard lg(_mutex);
        return _count;
    }
private:
    mutable Mutex _mutex;
    Condition _cond;
    int _count;
};

} // namespace mihooke

#endif