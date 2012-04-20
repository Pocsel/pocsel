#ifndef __SERVER_GAME_MAP_GEN_EQUATIONS_NONE_HPP__
#define __SERVER_GAME_MAP_GEN_EQUATIONS_NONE_HPP__

#include "server/game/map/gen/IEquation.hpp"

namespace Server { namespace Game { namespace Map { namespace Gen { namespace Equations {

    class None1 :
        public IEquation
    {
    public:
        explicit None1(std::map<std::string, double> const&)
        {
        }

        virtual void Calc(double, double, double, Uint32, Uint32, Uint32, double* res, unsigned int offset) const
        {
            for (unsigned int i = 0; i < Common::ChunkSize3; ++i)
            {
                *res = 0.0;
                res += offset;
            }
        }
    };

}}}}}

#endif

