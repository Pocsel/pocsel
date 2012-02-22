#ifndef __TOOLS_THREAD_TASK_HPP__
#define __TOOLS_THREAD_TASK_HPP__

namespace Tools { namespace Thread {

    class Task
    {
    private:
        std::function<void()> _task;
        bool _isFinish;

    public:
        Task(std::function<void()>&& task);

        void Execute();
        bool IsFinish() const { return this->_isFinish; }
    };

}}

#endif
