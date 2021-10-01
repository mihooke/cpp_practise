#include "Thread.h"
#include "Logging.h"
#include "CurrentThread.h"
#include "Exception.h"

#include <sys/prctl.h>

namespace mihooke
{

std::atomic<int> Thread::_numOfThreads;

Thread::Thread(threadFunc func, const std::string &name) :
    _isRunning(false), _pthreadId(0), _tid(0), _func(func), _name(name)
{
    int num = ++_numOfThreads;
    if (name.empty())
    {
        _name.append("Thread-");
        _name.append(std::to_string(num));
    }
}
Thread::~Thread()
{
    if (_isRunning)
    {
        pthread_detach(_pthreadId);
    }
}

void Thread::start()
{
    _isRunning = true;
    if (pthread_create(&_pthreadId, nullptr, &Thread::run, this))
    {
        _isRunning = false;
        LOG_FATAL << "Failed to create pthread";
    }
}

int Thread::join()
{
    return pthread_join(_pthreadId, nullptr);
}

void *Thread::run(void *arg)
{
    auto thread = static_cast<Thread*>(arg);
    thread->tid() = gettid();
    t_threadName = thread->name().c_str();
    ::prctl(PR_SET_NAME, t_threadName); // 设置进程名字
    try
    {
        thread->func()();
        t_threadName = "finished";
    }
    catch(const Exception& e)
    {
        fprintf(stderr, "exception caught in Thread %s\n", thread->name().c_str());
        fprintf(stderr, "reason: %s\n", e.what());
        fprintf(stderr, "exception message:: %s\n", e.msg().c_str());
        fprintf(stderr, "stack trace: \n%s\n", e.stack().c_str());
        t_threadName = "crashed";
        abort();
    }
    catch(const std::exception& e)
    {
        fprintf(stderr, "exception caught in Thread %s\n", thread->name().c_str());
        fprintf(stderr, "reason: %s\n", e.what());
        fprintf(stderr, "stack trace: \n%s\n", stackTrace().c_str());
        t_threadName = "crashed";
        abort();
    }
    catch (...)
    {
        t_threadName = "crashed";
        fprintf(stderr, "unknown exception caught in Thread %s\n", thread->name().c_str());
        throw;
    }
    return nullptr;
}
} // namespace mihooke
