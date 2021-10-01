#ifndef MIHOOKE_CURRENT_THREAD_H
#define MIHOOKE_CURRENT_THREAD_H

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/prctl.h>
#include <linux/unistd.h>
#include <string>

namespace mihooke
{

extern __thread int t_catchedTid;
extern __thread const char *t_threadName;

inline int gettid()
{
    // gcc指令，最可能的值
    if (__builtin_expect(t_catchedTid == 0, 0))
    {
        t_catchedTid = ::syscall(SYS_gettid);
    }
    return t_catchedTid;
}

inline const char *threadName()
{
    return t_threadName;
}

std::string stackTrace();

} // namespace mihooke


#endif