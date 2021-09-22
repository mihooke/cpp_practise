#include "../base/Condition.h"
#include "../base/Logging.h"
#include <thread>
#include <chrono>

static int g_i{0};
mihooke::Mutex mutex;
mihooke::Condition cond(mutex);

using namespace std::chrono_literals;

void funcProducer()
{
    LOG_INFO << "Producer begins to work.";
    for (int i=0; i<1000000; ++i)
        g_i ++;
    std::this_thread::sleep_for(2000ms);
    cond.notify();
    LOG_INFO << "Producer calcute finished, sleeping...";
    std::this_thread::sleep_for(2000ms);
    LOG_INFO << "Producer exit.";
}

void funcConsumer()
{
    LOG_INFO << "Consumer waiting...";
    cond.wait();
    LOG_INFO << "Consumer begins to rework";
    g_i *= 2;
    LOG_INFO << g_i ;
}

void funcConsumer2()
{
    LOG_INFO << "Consumer waiting...";
    bool ret = cond.wait(1);
    
    LOG_INFO << (ret ? "Timeout! No rework." : "Consumer begins to rework");
    if (!ret)
        g_i *= 2;
}


int main()
{
    std::thread t1(funcProducer);
    std::thread t2(funcConsumer2);
    t1.join();
    t2.join();
    LOG_INFO << g_i ;
    return 0;
}