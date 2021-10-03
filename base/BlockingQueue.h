#ifndef MIHOOKE_BLOCKINGQUEUE_H
#define MIHOOKE_BLOCKINGQUEUE_H

#include <deque>
#include <functional>
#include "Condition.h"

namespace mihooke
{

template<typename T>
class BlockingQueue
{
public:
    BlockingQueue() : _mutex(), _notEmpty(_mutex) {}

    void push(const T &t)
    {
        MutexLockGuard lg(_mutex);
        _queue.push_back(t);
        _notEmpty.notify();
    }
    void push(T &&t)
    {
        MutexLockGuard lg(_mutex);
        _queue.push_back(std::move(t));
        _notEmpty.notify();
    }
    T take()
    {
        MutexLockGuard lg(_mutex);
        while (_queue.empty())
        {
            _notEmpty.wait();
        }
        T t{std::move(_queue.front())};
        _queue.pop_front();
        return std::move(t);
    }
    size_t size() const
    {
        MutexLockGuard lg(_mutex);
        return _queue.size();
    }
private:
    Mutex _mutex;
    Condition _notEmpty;
    std::deque<T> _queue;
};

} // namespace mihooke


#endif