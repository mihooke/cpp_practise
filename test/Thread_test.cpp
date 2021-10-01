#include "../base/Thread.h"
#include "../base/Logging.h"
#include "../base/Mutex.h"
#include "../base/CurrentThread.h"
#include "../base/Exception.h"
#include <pthread.h>

using namespace mihooke;

static int g_i{0};
mihooke::Mutex mutex;

void throwExceptionTest()
{
    throw Exception("An std::exception");
}

void func()
{
    LOG_INFO<<"TID="<<t_catchedTid<<t_threadName;
    mihooke::MutexLockGuard lg(mutex);
    for (int i=0; i<1000000; ++i)
        g_i ++;
    throwExceptionTest();
}
void func1()
{
    LOG_INFO<<"TID="<<t_catchedTid<<t_threadName;
    mihooke::MutexLockGuard lg(mutex);
    for (int i=0; i<1000000; ++i)
        g_i ++;
}

int main()
{
    Thread t1(func1, "t1");
    Thread t2(func1);
    t1.start();
    t2.start();
    t1.join();
    t2.join();
    LOG_INFO << g_i ;
    return 0;
}