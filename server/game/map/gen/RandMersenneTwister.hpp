#ifndef __SERVER_GAME_MAP_GEN_RANDMERSENNETWISTER_HPP__
#define __SERVER_GAME_MAP_GEN_RANDMERSENNETWISTER_HPP__

#include "server/game/map/gen/IRand.hpp"

namespace Server { namespace Game { namespace Map { namespace Gen {

    class RandMersenneTwister
        : public IRand
    {
    private:
        Int32 _tab[624];
        Int32 _index;

    public:
        RandMersenneTwister(Int32 seed);
        virtual ~RandMersenneTwister();
        virtual Int32 Rand();
        virtual void ReSeed(Int32 seed);
    private:
        void _GenerateNumbers();
        void _ReSeed(Int32 seed);
    };

}}}}

#endif
