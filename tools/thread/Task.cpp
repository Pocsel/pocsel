#include "tools/precompiled.hpp"

#include "tools/thread/Task.hpp"

namespace Tools { namespace Thread {

    Task::Task(std::function<void()>&& task)
        : _task(task),
        _isFinish(false)
    {
    }

    void Task::Execute()
    {
        this->_task();
        this->_isFinish = true;
    }

}}