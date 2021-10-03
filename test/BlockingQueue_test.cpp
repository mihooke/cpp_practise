#include "../base/BlockingQueue.h"
#include "../base/Logging.h"
#include "../base/Thread.h"
#include <unistd.h>

using namespace mihooke;

BlockingQueue<int> queue;

void consumer()
{
    while (true)
    {
        LOG_INFO << "Waiting...";
        int v = queue.take();
        LOG_INFO << "Get a value:" << v;
    }
}

int main()
{
    Thread thread(consumer, "QueueComsumerThread");
    thread.start();
    for (int i=1; i<4; ++i)
    {
        queue.push(i);
        sleep(i);
    }
    while (true)
    {
        sleep(10);
    }
    
    return 0;
}