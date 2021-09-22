#include "../base/Mutex.h"
#include "../base/Logging.h"
#include <thread>

static int g_i{0};
mihooke::Mutex mutex;

void func()
{
    mihooke::MutexLockGuard lg(mutex);
    for (int i=0; i<1000000; ++i)
        g_i ++;
}

int main()
{
    std::thread t1(func);
    std::thread t2(func);
    t1.join();
    t2.join();
    LOG_INFO << g_i ;
    return 0;
}
