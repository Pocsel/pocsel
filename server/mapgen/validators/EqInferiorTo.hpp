#ifndef __SERVER_MAPGEN_VALIDATORS_EQINFERIORTO_HPP__
#define __SERVER_MAPGEN_VALIDATORS_EQINFERIORTO_HPP__

#include "server/mapgen/Validator.hpp"

namespace Server { namespace MapGen { namespace Validators {

    class EqInferiorTo : public Validator
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

}}}

#endif
