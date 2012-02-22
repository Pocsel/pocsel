#ifndef __TOOLS_THREAD_THREADPOOL_HPP__
#define __TOOLS_THREAD_THREADPOOL_HPP__

namespace Tools { namespace Thread {

    class Task;

    class ThreadPool
    {
    private:
        std::list<boost::thread*> _threads;
        boost::mutex _conditionMutex;
        boost::condition_variable _condition;
        boost::mutex _taskMutex;
        std::list<std::function<void()>> _tasks;
        bool _isRunning;

    public:
        ThreadPool(int nbThreads = 1);
        ~ThreadPool();

        void PushTask(std::function<void()>&& task);
        void PushTask(std::shared_ptr<Tools::Thread::Task> task);
    private:
        void _Run();
    };

}}

#endif
