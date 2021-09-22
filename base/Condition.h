#ifndef MIHOOKE_CONDITION_H
#define MIHOOKE_CONDITION_H

#include "Mutex.h"
#include <pthread.h>
#include <stdint.h>
#include <errno.h>

namespace mihooke
{

class Condition
{
public:
    explicit Condition(Mutex &mutex) : _mutex(mutex)
    {
        pthread_cond_init(&_cond, nullptr);
    }

    ~Condition()
    {
        pthread_cond_destroy(&_cond);
    }

    void wait()
    {
        pthread_cond_wait(&_cond, _mutex.mutex());
    }

    bool wait(double seconds)
    {
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        const int64_t nanoSecondsPerSecond = 1000 * 1000 * 1000;
        int64_t nanoSeconds = static_cast<int64_t>(seconds * nanoSecondsPerSecond);
        abstime.tv_sec += static_cast<time_t>((abstime.tv_nsec + nanoSeconds) / nanoSecondsPerSecond);
        abstime.tv_nsec = static_cast<long>((abstime.tv_nsec + nanoSeconds) % nanoSecondsPerSecond);

        return ETIMEDOUT == pthread_cond_timedwait(&_cond, _mutex.mutex(), &abstime);
    }

    void notify()
    {
        pthread_cond_signal(&_cond);
    }

    void notifyAll()
    {
        pthread_cond_broadcast(&_cond);
    }
private:
    Mutex &_mutex;
    pthread_cond_t _cond;
};

} // namespace mihooke


#endif