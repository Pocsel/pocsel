#ifndef __SERVER_GAME_MAP_GEN_VALIDATORS_EQINFERIORTO_HPP__
#define __SERVER_GAME_MAP_GEN_VALIDATORS_EQINFERIORTO_HPP__

#include "server2/game/map/gen/IValidator.hpp"

namespace Server { namespace Game { namespace Map { namespace Gen { namespace Validators {

    class EqInferiorTo :
        public IValidator
    {
    private:
        double _value;

    public:
        explicit EqInferiorTo(std::map<std::string, double> const& values)
        {
            if (values.find("value") != values.end())
                _value = values.find("value")->second;
            else
            {
                _value = 0;
                std::cout << "WARNING: value 'value' not found for 'Inferior', default used (0)\n";
            }
        }

        virtual bool Check(double, double, double, double eq) const
        {
            return eq < _value;
        }
    };

}}}}}

#endif
