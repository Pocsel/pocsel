#ifndef __TOOLS_THREAD_TASK_HPP__
#define __TOOLS_THREAD_TASK_HPP__

namespace Tools { namespace Thread {

    class ITask
    {
    public:
        virtual ~ITask() {}
        virtual void Execute() = 0;
        virtual bool IsCancelled() const = 0;
    };

    template<class T = void>
    class Task : public ITask
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

        virtual void Execute()
        {
            if (!this->_isCancelled)
                this->_result = this->_task();
            this->_isExecuted = true;
        }
        bool IsExecuted() const { return this->_isExecuted; }
        T const& GetResult() const { return this->_result; }
        T& GetResult() { return this->_result; }
        virtual bool IsCancelled() const { return this->_isCancelled; }
        void Cancel() { this->_isCancelled = true; }
    };

    template<>
    class Task<void> : public ITask
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

        virtual void Execute()
        {
            if (!this->_isCancelled)
                this->_task();
            this->_isExecuted = true;
        }
        bool IsExecuted() const { return this->_isExecuted; }
        virtual bool IsCancelled() const { return this->_isCancelled; }
        void Cancel() { this->_isCancelled = true; }
    };

}}

#endif
