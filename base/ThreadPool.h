#ifndef MIHOOKE_THREADPOOL_H
#define MIHOOKE_THREADPOOL_H

#include <vector>
#include <deque>
#include <functional>
#include <memory>

#include "Thread.h"
#include "Condition.h"
#include "Mutex.h"

namespace mihooke
{

class ThreadPool
{
public:
    using Task = std::function<void ()>;
    ThreadPool(size_t num, const std::string &name="ThreadPool");
    ~ThreadPool();
    void start();
    void stop();
    void run(Task t);

    const std::string &name() const {return _name;}
private:
    bool isFull() const;
    void loop();

private:
    bool _isRunning{false};
    size_t _maxNum;
    std::string _name;
    Mutex _mutex;
    Condition _notFull;
    Condition _notEmpty;
    std::vector<std::unique_ptr<Thread>> _threads;
    std::deque<Task> _tasks;
};

} // namespace mihooke


#endif