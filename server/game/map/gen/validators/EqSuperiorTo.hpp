#ifndef __SERVER_GAME_MAP_GEN_VALIDATORS_EQSUPERIORTO_HPP__
#define __SERVER_GAME_MAP_GEN_VALIDATORS_EQSUPERIORTO_HPP__

#include "server/game/map/gen/IValidator.hpp"

namespace Server { namespace Game { namespace Map { namespace Gen { namespace Validators {

    class EqSuperiorTo :
        public IValidator
    {
    private:
        double _value;

    public:
        explicit EqSuperiorTo(std::map<std::string, double> const& values)
        {
            if (values.find("value") != values.end())
                _value = values.find("value")->second;
            else
            {
                _value = 0;
                std::cout << "WARNING: value 'value' not found for 'superior', default used (0)\n";
            }
        }

        virtual bool Check(double, double, double, double eq) const
        {
            return eq > _value;
        }
    };

}}}}}

#endif
