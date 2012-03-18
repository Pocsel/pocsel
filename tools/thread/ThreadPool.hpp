#ifndef __TOOLS_THREAD_THREADPOOL_HPP__
#define __TOOLS_THREAD_THREADPOOL_HPP__

#include "tools/thread/Task.hpp"

namespace Tools { namespace Thread {

    class ThreadPool
    {
    private:
        std::list<boost::thread*> _threads;
        boost::mutex _conditionMutex;
        boost::condition_variable _condition;
        boost::mutex _taskMutex;
        std::list<std::shared_ptr<ITask>> _tasks;
        bool _isRunning;

    public:
        ThreadPool(int nbThreads = 1);
        ~ThreadPool();

        void PushTask(std::function<void()>&& task);
        void PushTask(std::shared_ptr<ITask> task);
        template<class T>
        void PushTask(std::shared_ptr<Task<T>> task)
        {
            this->PushTask(std::shared_ptr<ITask>(task));
        }
    private:
        void _Run();
    };

}}

#endif
