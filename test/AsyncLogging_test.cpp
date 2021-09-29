#include "../base/AsyncLogging.h"
#include "../base/Logging.h"
#include <thread>
#include <chrono>

using namespace mihooke;
using namespace std::chrono_literals;

AsyncLogging *g_async_logging = nullptr;

void output(const char *msg, int len)
{
    g_async_logging->append(msg, len);
}

int main()
{
    AsyncLogging a_logging("log_test.log");
    g_async_logging = &a_logging;
    g_async_logging->start();
    Logging::setOutput(output);
    LOG_INFO << "s message";
    std::this_thread::sleep_for(1s);
    return 0;
}