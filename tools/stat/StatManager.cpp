#include "tools/precompiled.hpp"

#include "tools/stat/StatManager.hpp"

namespace Tools { namespace Stat {

    StatManager statManager;
    static bool _destroyed = false;

    StatManager::StatManager() :
        _ticks(0)
    {
    }

    StatManager::~StatManager()
    {
        _destroyed = true;
    }

    void StatManager::Register(IStat& stat)
    {
#ifdef DEBUG
        this->_counters.push_back(std::make_pair(&stat, std::list<double>()));
#else
        this->_counters.push_back(std::make_pair(&stat, 0.0));
#endif
    }

    void StatManager::Unregister(IStat& stat)
    {
        if (_destroyed)
            return;
        auto it = this->_counters.begin();
        while (it != this->_counters.end() && it->first != &stat)
            ++it;
        if (it != this->_counters.end())
            this->_counters.erase(it);
    }

    void StatManager::Update()
    {
        ++this->_ticks;
        for (auto it = this->_counters.begin(), ite = this->_counters.end(); it != ite; ++it)
        {
            double value = it->first->GetValue();
#ifdef DEBUG
            it->second.push_back(value);
#else
            it->second = value;
#endif
        }
    }
}}
