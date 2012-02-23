#ifndef __TOOLS_THREAD_THREADPOOL_HPP__
#define __TOOLS_THREAD_THREADPOOL_HPP__

namespace Tools { namespace Thread {

    template<class T> class Task;

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
        template<class T>
        void PushTask(std::weak_ptr<Tools::Thread::Task<T>> task)
        {
            this->PushTask(
                [task]()
                {
                    auto ptr = task.lock();
                    if (ptr)
                        ptr->Execute();
                });
        }
    private:
        void _Run();
    };

}}

#endif
