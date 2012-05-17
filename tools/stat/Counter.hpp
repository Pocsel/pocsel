#ifndef __TOOLS_STAT_COUNTER_HPP__
#define __TOOLS_STAT_COUNTER_HPP__

#include "tools/Timer.hpp"
#include "tools/stat/IStat.hpp"
#include "tools/stat/StatManager.hpp"

namespace Tools { namespace Stat {

    class Counter : public IStat
    {
    private:
        std::string _name;
        Tools::Timer _timer;
        double _value;

    public:
        Counter(std::string name) : _name(name), _value(0) { Tools::Stat::statManager.Register(*this); }
        ~Counter() { Tools::Stat::statManager.Unregister(*this); }

        Counter& operator +=(double value) { this->_value += value; return *this; }
        Counter& operator -=(double value) { this->_value -= value; return *this; }
        void Reset(double value = 0) { this->_value = value; }

        virtual std::string const& GetName() const { return this->_name; }
        virtual double GetValue() const { return this->_value; }
    };

    class FakeCounter
    {
    private:
        std::string _name;

    public:
        FakeCounter(std::string name) : _name(name) {}

        FakeCounter& operator +=(double) { return *this; }
        FakeCounter& operator -=(double) { return *this; }
        void Reset(double = 0) {}

        std::string const& GetName() const { return this->_name; }
        double GetValue() const { return 0.0; }
    };

#ifdef DEBUG
    typedef Counter DebugCounter;
#else
    typedef FakeCounter DebugCounter;
#endif
}}

#endif
