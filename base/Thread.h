#ifndef MIHOOKE_THREAD_H
#define MIHOOKE_THREAD_H

#include <functional>
#include <pthread.h>
#include <atomic>

namespace mihooke
{

class Thread
{
public:
    using threadFunc = std::function<void ()>;
    explicit Thread(threadFunc func, const std::string &name="");
    ~Thread();

    void start();
    int join();

    const std::string &name() const {return _name;}
    pid_t &tid() {return _tid;}
    threadFunc &func() {return _func;}

private:
    static void *run(void *arg);
private:
    bool _isRunning;
    pthread_t _pthreadId;
    pid_t _tid;
    threadFunc _func;
    std::string _name;

    static std::atomic<int> _numOfThreads;
};

} // namespace mihooke


#endif