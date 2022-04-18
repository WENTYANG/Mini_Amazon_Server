#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <functional>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

using namespace std;


// a thread safe thread pool
class Threadpool
{
private:
    vector<std::thread> threads;
    queue<function<void()> > m_tasks;
    int max_threads;
    int max_tasks;
    mutex m_lock;
    condition_variable has_task;
    // use to determine whether recieved a task or the treadpool is destructed
    bool run_flag;
public:
    Threadpool(int max_threads=50, int max_tasks=600);
    bool init_pool(function<void()> t);
    Threadpool * get_pool();
    bool assign_task(function<void()> task);
    ~Threadpool();
private:
    bool init_threads();
    void run_task();//take the first task in queue and excute



};

#endif
