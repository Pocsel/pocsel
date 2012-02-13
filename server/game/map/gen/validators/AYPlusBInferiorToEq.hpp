#ifndef __SERVER_GAME_MAP_GEN_VALIDATORS_AYPLUSBINFERIORTOEQ_HPP__
#define __SERVER_GAME_MAP_GEN_VALIDATORS_AYPLUSBINFERIORTOEQ_HPP__

#include "server/game/map/gen/IValidator.hpp"

namespace Server { namespace Game { namespace Map { namespace Gen { namespace Validators {

    class AYPlusBInferiorToEq :
        public IValidator
    {
    private:
        double _a;
        double _b;

    public:
        explicit AYPlusBInferiorToEq(std::map<std::string, double> const& values)
        {
            if (values.find("a") != values.end())
                _a = values.find("a")->second;
            else
            {
                _a = 1;
                std::cout << "WARNING: value 'a' not found for 'AYPlusBInferiorToEq', default used (1)\n";
            }
            if (values.find("b") != values.end())
                _b = values.find("b")->second;
            else
            {
                _b = 0;
                std::cout << "WARNING: value 'b' not found for 'AYPlusBInferiorToEq', default used (0)\n";
            }
        }

        virtual bool Check(double, double y, double, double eq) const
        {
            return _a * y + _b < eq;
        }
    };

}}}}}

#endif
