#include "ThreadPool.h"
#include "Exception.h"
#include "Logging.h"
#include "CurrentThread.h"

namespace mihooke
{

ThreadPool::ThreadPool(size_t num, const std::string &name) :
    _maxNum(num), _name(name), _mutex(), _notFull(_mutex), _notEmpty(_mutex)
{
    _threads.reserve(num);
}

ThreadPool::~ThreadPool()
{
    if (_isRunning)
    {
        stop();
    }
}

void ThreadPool::start()
{
    _isRunning =true;
    for (int i = 0; i < _maxNum; ++i)
    {
        char id[16];
        snprintf(id, sizeof(id), " Thread-%d", i+1);
        _threads.emplace_back(new Thread(std::bind(&ThreadPool::loop, this), _name+id));
        _threads[i]->start();
    }
}

void ThreadPool::stop()
{
    {
        MutexLockGuard lg(_mutex);
        _isRunning = false;
        _notEmpty.notifyAll();
    }
    for (int i = 0; i < _maxNum; ++i)
    {
        _threads[i]->join();
    }
}

void ThreadPool::run(Task t)
{
    if (_maxNum == 0)
    {
        t();
        return;
    }
    MutexLockGuard lg(_mutex);
    while (isFull())
    {
        _notFull.wait();
    }
    _tasks.push_front(t);
    _notEmpty.notify(); // 唤醒一个thread即可
}

bool ThreadPool::isFull() const
{
    return _tasks.size() >= _maxNum;
}

void ThreadPool::loop()
{
    try
    {
        LOG_TRACE << threadName() << "started";
        while (_isRunning)
        {
            Task task;
            {
                MutexLockGuard lg(_mutex);
                while (_tasks.empty() && _isRunning)
                {
                    _notEmpty.wait();
                }
                task = _tasks.back();
                _tasks.pop_back();
                _notFull.notify();
            }
            if (task)
            {
                LOG_TRACE << threadName() << "run a task";
                task();
                LOG_TRACE << threadName() << "finished task";
            }
        }
    }
    catch(const Exception& e)
    {
        fprintf(stderr, "exception caught in ThreadPool %s\n", _name.c_str());
        fprintf(stderr, "reason: %s\n", e.what());
        fprintf(stderr, "exception message:: %s\n", e.msg().c_str());
        fprintf(stderr, "stack trace: \n%s\n", e.stack().c_str());
        t_threadName = "crashed";
        abort();
    }
    catch(const std::exception& e)
    {
        fprintf(stderr, "exception caught in ThreadPool %s\n", _name.c_str());
        fprintf(stderr, "reason: %s\n", e.what());
        fprintf(stderr, "stack trace: \n%s\n", stackTrace().c_str());
        t_threadName = "crashed";
        abort();
    }
    catch (...)
    {
        t_threadName = "crashed";
        fprintf(stderr, "unknown exception caught in ThreadPool %s\n", _name.c_str());
        throw;
    }
}

} // namespace mihooke
