#include "../base/Logging.h"


int main()
{
    LOG_TRACE<<"TRACE MSG.";
    {
        LOG_DEBUG<<"DEBUG msg.";
    }
    LOG_DEBUG << 1 << 1.3 << true;

    LOG_INFO<<"INFO";
    LOG_WARNING<<"WARNING";
    // LOG_ERROR<<"ERROR";
    // LOG_FATAL<<"FATAL";
    return 0;
}
