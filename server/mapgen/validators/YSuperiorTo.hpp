#ifndef __SERVER_MAPGEN_VALIDATORS_YSUPERIORTO_HPP__
#define __SERVER_MAPGEN_VALIDATORS_YSUPERIORTO_HPP__

#include "server/mapgen/Validator.hpp"

namespace Server { namespace MapGen { namespace Validators {

    class YSuperiorTo : public Validator
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

}}}

#endif
