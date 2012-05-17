#ifndef __TOOLS_STAT_STATMANAGER_HPP__
#define __TOOLS_STAT_STATMANAGER_HPP__

#include "tools/stat/IStat.hpp"

namespace Tools { namespace Stat {

    class StatManager
    {
    private:
        int _ticks;
#ifdef DEBUG
        std::list<std::pair<IStat*, std::list<double>>> _counters;
#else
        std::list<std::pair<IStat*, double>> _counters;
#endif

    public:
        StatManager();
        ~StatManager();

        void Register(IStat& counter);
        void Unregister(IStat& counter);

#ifdef DEBUG
        std::list<std::pair<IStat*, double>> GetStats() const
        {
            std::list<std::pair<IStat*, double>> counters;
            for (auto it = this->_counters.begin(), ite = this->_counters.end(); it != ite; ++it)
                counters.push_back(std::make_pair(it->first, it->second.front()));
            return counters;
        }
        std::list<std::pair<IStat*, std::list<double>>> const& GetDebugStats() const { return this->_counters; }
#else
        std::list<std::pair<IStat*, double>> const& GetStats() const { return this->_counters; }
        std::list<std::pair<IStat*, std::list<double>>> GetDebugStats() const { return std::vector<std::pair<IStat*, std::list<double>>>(); }
#endif
        // Met à jour les stats (à appeler par tick ou frame)
        void Update();
    };

    extern StatManager statManager;

}}

#endif
