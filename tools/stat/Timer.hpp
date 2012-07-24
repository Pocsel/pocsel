#ifndef __TOOLS_STAT_TIMER_HPP__
#define __TOOLS_STAT_TIMER_HPP__

#include <string>
#include "tools/types.hpp"
#include "tools/Timer.hpp"
#include "tools/stat/IStat.hpp"
#include "tools/stat/StatManager.hpp"

namespace Tools { namespace Stat {

    class Timer : public IStat
    {
    private:
        std::string _name;
        Tools::Timer _timer;
        double _value;

    public:
        Timer(std::string name) : _name(name), _value(0) { Tools::Stat::statManager.Register(*this); }
        ~Timer() { Tools::Stat::statManager.Unregister(*this); }

        void Reset() { this->_value = 0; }
        void Begin() { this->_timer.Reset(); }
        void Continue() { this->_timer.Set(Uint64(this->_value * 1000000.0)); }
        void End() { this->_value = (double)this->_timer.GetPreciseElapsedTime() / 1000000.0; }

        virtual std::string const& GetName() const { return this->_name; }
        virtual double GetValue() const { return this->_value; }
    };

    class FakeTimer
    {
    private:
        std::string _name;

    public:
        FakeTimer(std::string name) : _name(name) {}
        void Reset() {}
        void Begin() {}
        void Continue() {}
        void End() {}
        virtual std::string const& GetName() const { return this->_name; }
        virtual double GetValue() const { return 0.0; }
    };

#ifdef DEBUG
    typedef Timer DebugTimer;
#else
    typedef FakeTimer DebugTimer;
#endif
}}

#endif
