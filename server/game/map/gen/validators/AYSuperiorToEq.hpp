#ifndef __SERVER_GAME_MAP_GEN_VALIDATORS_AYSUPERIORTOEQ_HPP__
#define __SERVER_GAME_MAP_GEN_VALIDATORS_AYSUPERIORTOEQ_HPP__

#include "server2/game/map/gen/IValidator.hpp"

namespace Server { namespace Game { namespace Map { namespace Gen { namespace Validators {

    class AYSuperiorToEq :
        public IValidator
    {
    private:
        double _a;

    public:
        explicit AYSuperiorToEq(std::map<std::string, double> const& values)
        {
            if (values.find("a") != values.end())
                _a = values.find("a")->second;
            else
            {
                _a = 1;
                std::cout << "WARNING: value 'a' not found for 'AYSuperiorToEq', default used (1)\n";
            }
        }

        virtual bool Check(double, double y, double, double eq) const
        {
            return _a * y > eq;
        }
    };

}}}}}

#endif
