#include "../base/CountDown.h"
#include "../base/Logging.h"
#include <thread>
#include <chrono>

mihooke::CountDown countDown(3);

void func(int seconds)
{
    auto a = std::chrono::duration<int>(seconds);
    std::this_thread::sleep_for(a);
    LOG_INFO << "Down" <<seconds;
    LOG_INFO << countDown.count();
    countDown.down();
}

int main()
{
    std::thread t1(func, 1);
    std::thread t2(func, 2);
    std::thread t3(func, 3);
    LOG_INFO << "waiting...";
    countDown.wait();
    LOG_INFO << "waiting finished";
    t1.join();
    t2.join();
    t3.join();
    return 0;
}