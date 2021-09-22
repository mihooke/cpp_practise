#ifndef MIHOOKE_MUTEX_H
#define MIHOOKE_MUTEX_H

#include <pthread.h>

namespace mihooke
{

class Mutex
{
public:
    Mutex()
    {
        pthread_mutex_init(&_mutex, nullptr);
    }

    ~Mutex()
    {
        pthread_mutex_destroy(&_mutex);
    }

    void lock()
    {
        pthread_mutex_lock(&_mutex);
    }

    void unlock()
    {
        pthread_mutex_unlock(&_mutex);
    }

    pthread_mutex_t *mutex()
    {
        return &_mutex;
    }
private:
    pthread_mutex_t _mutex;
};

class MutexLockGuard
{
public:
    explicit MutexLockGuard(Mutex &mutex) : _mutex(mutex)
    {
        _mutex.lock();
    }
    ~MutexLockGuard()
    {
        _mutex.unlock();
    }
private:
    Mutex &_mutex;
};

} // namespace mihooke

#endif