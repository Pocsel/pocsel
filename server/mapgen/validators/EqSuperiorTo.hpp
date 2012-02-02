#ifndef __SERVER_MAPGEN_VALIDATORS_EQSUPERIORTO_HPP__
#define __SERVER_MAPGEN_VALIDATORS_EQSUPERIORTO_HPP__

#include "server/mapgen/Validator.hpp"

namespace Server { namespace MapGen { namespace Validators {

    class EqSuperiorTo : public Validator
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

}}}

#endif
