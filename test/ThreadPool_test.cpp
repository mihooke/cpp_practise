#include "../base/ThreadPool.h"
#include "../base/Logging.h"
#include <thread>
#include <chrono>

using namespace mihooke;
using namespace std::chrono_literals;

void task1()
{
    LOG_INFO << "task1, will cost 10 seconds";
    std::this_thread::sleep_for(10s);
}

void task2(int seconds)
{
    LOG_INFO << "task2, will cost " << seconds << " seconds";
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void task3(int arg1, int arg2)
{
    LOG_INFO << "task3, will cost 1 seconds";
    std::this_thread::sleep_for(1s);
}

int main()
{
    ThreadPool pool(4);
    pool.start();
    pool.run(task1);
    pool.run(std::bind(task2, 20));
    pool.run(std::bind(task3, 1, 2));
    pool.run(std::bind(task3, 1, 3));
    pool.run(std::bind(task3, 1, 3));
    while (true)
    {
        std::this_thread::sleep_for(2s);
    }
    return 0;
}
