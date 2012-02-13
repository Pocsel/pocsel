#ifndef __SERVER_GAME_MAP_GEN_VALIDATORS_EQINFERIORTOY_HPP__
#define __SERVER_GAME_MAP_GEN_VALIDATORS_EQINFERIORTOY_HPP__

#include "server/game/map/gen/IValidator.hpp"

namespace Server { namespace Game { namespace Map { namespace Gen { namespace Validators {

    class EqInferiorToY :
        public IValidator
    {
    public:
        explicit EqInferiorToY(std::map<std::string, double> const&)
        {
        }

        virtual bool Check(double, double y, double, double eq) const
        {
            return eq < y;
        }
    };

}}}}}

#endif
