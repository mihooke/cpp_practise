#include "CurrentThread.h"

#include <execinfo.h>
#include <cxxabi.h>
#include <string>

namespace mihooke
{

__thread int t_catchedTid{0};
__thread const char *t_threadName{"unknow"};

std::string stackTrace()
{
    std::string infos;
    constexpr int maxFrameSize{200};
    void *frame[maxFrameSize];
    int nptrs = ::backtrace(frame, maxFrameSize); // 获取函数调用地址
    char **symbols = ::backtrace_symbols(frame, nptrs); // 函数地址转为编译符号
    if (symbols)
    {
        size_t len{256};
        char *demangled = static_cast<char*>(malloc(len));
        for (int i = 1; i < nptrs; ++i)
        {
            char *leftPar = nullptr;
            char *plus = nullptr;
            for (char *p = symbols[i]; *p; ++p)
            {
                if (*p == '(')
                {
                    leftPar = p;
                }
                else if (*p == '+')
                {
                    plus = p;
                }
            }
            if (leftPar && plus)
            {
                *plus = '\0';
                int status{0};
                // 根据编译符号获取具体函数方法
                char *ret = abi::__cxa_demangle(leftPar+1, demangled, &len, &status);
                *plus = '+';
                if (status == 0)
                {
                    demangled = ret;
                    infos.append(symbols[i], leftPar+1);
                    infos.append(demangled);
                    infos.append(plus);
                    infos.append("\n");
                }
            }
        }
        free(demangled);
        free(symbols);
    }
    return infos;
}

} // namespace mihooke
