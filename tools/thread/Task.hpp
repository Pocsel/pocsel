#ifndef __TOOLS_THREAD_TASK_HPP__
#define __TOOLS_THREAD_TASK_HPP__

namespace Tools { namespace Thread {

    template<class T = void>
    class Task
    {
    private:
        std::function<T()> _task;
        bool _isCancelled;
        bool _isExecuted;
        T _result;

    public:
        Task(std::function<T()>&& task, T const& defaultValue = T())
            : _task(task),
            _isCancelled(false),
            _isExecuted(false),
            _result(defaultValue)
        {
        }

        void Execute()
        {
            if (!this->_isCancelled)
                this->_result = this->_task();
            this->_isExecuted = true;
        }
        bool IsExecuted() const { return this->_isExecuted; }
        T const& GetResult() const { return this->_result; }
        T& GetResult() { return this->_result; }
        bool IsCancelled() const { return this->_isCancelled; }
        void Cancel() { this->_isCancelled = true; }
    };

    template<>
    class Task<void>
    {
    private:
        std::function<void()> _task;
        bool _isCancelled;
        bool _isExecuted;

    public:
        Task(std::function<void()>&& task)
            : _task(task),
            _isCancelled(false),
            _isExecuted(false)
        {
        }

        void Execute()
        {
            if (!this->_isCancelled)
                this->_task();
            this->_isExecuted = true;
        }
        bool IsExecuted() const { return this->_isExecuted; }
        bool IsCancelled() const { return this->_isCancelled; }
        void Cancel() { this->_isCancelled = true; }
    };

}}

#endif
