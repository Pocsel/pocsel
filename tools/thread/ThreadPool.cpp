#include "tools/precompiled.hpp"

#include "tools/thread/Task.hpp"
#include "tools/thread/ThreadPool.hpp"

namespace Tools { namespace Thread {

    ThreadPool::ThreadPool(int nbThreads)
        : _isRunning(true)
    {
        for (int i = 0; i < nbThreads; ++i)
            this->_threads.push_back(new boost::thread(std::bind(&ThreadPool::_Run, this)));
    }

    ThreadPool::~ThreadPool()
    {
        this->_isRunning = false;
        this->_condition.notify_all();
        {
            boost::lock_guard<boost::mutex> lock(this->_taskMutex);
            this->_tasks.clear();
        }
        for (auto it = this->_threads.begin(), ite = this->_threads.end(); it != ite; ++it)
        {
            (*it)->join();
            Tools::Delete(*it);
        }
    }

    void ThreadPool::PushTask(std::function<void()>&& task)
    {
        this->PushTask(std::shared_ptr<ITask>(new Task<void>(std::move(task))));
    }

    void ThreadPool::PushTask(std::shared_ptr<ITask> task)
    {
        boost::lock_guard<boost::mutex> lock(this->_taskMutex);
        this->_tasks.emplace_back(task);
        this->_condition.notify_one();
    }

    void ThreadPool::_Run()
    {
        while (this->_isRunning)
        {
            std::shared_ptr<ITask> task;
            {
                boost::lock_guard<boost::mutex> lock(this->_taskMutex);
                this->_tasks.remove_if(
                    [](std::shared_ptr<ITask> task)
                    {
                        return task->IsCancelled();
                    });
                if (!this->_tasks.empty())
                {
                    task = this->_tasks.front();
                    this->_tasks.pop_front();
                }
            }
            if (task.get() != 0)
                task->Execute();
            else
            {
                boost::unique_lock<boost::mutex> conditionLock(this->_conditionMutex);
                this->_condition.wait(conditionLock);
            }
        }
    }
}}