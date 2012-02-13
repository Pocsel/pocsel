#ifndef __SERVER_GAME_MAP_GEN_VALIDATORS_YSUPERIORTO_HPP__
#define __SERVER_GAME_MAP_GEN_VALIDATORS_YSUPERIORTO_HPP__

#include "server/game/map/gen/IValidator.hpp"

namespace Server { namespace Game { namespace Map { namespace Gen { namespace Validators {

    class YSuperiorTo :
        public IValidator
    {
    private:
        double _value;

    public:
        explicit YSuperiorTo(std::map<std::string, double> const& values)
        {
            if (values.find("value") != values.end())
                _value = values.find("value")->second;
            else
            {
                _value = 0;
                std::cout << "WARNING: value 'value' not found for 'Superior', default used (0)\n";
            }
        }

        virtual bool Check(double, double y, double, double) const
        {
            return y > _value;
        }
    };

}}}}}

#endif
