#ifndef __SERVER_MAPGEN_VALIDATORS_AYINFERIORTOEQ_HPP__
#define __SERVER_MAPGEN_VALIDATORS_AYINFERIORTOEQ_HPP__

#include "server/mapgen/Validator.hpp"

namespace Server { namespace MapGen { namespace Validators {

    class AYInferiorToEq : public Validator
    {
        private:
            double _a;

        public:
            explicit AYInferiorToEq(std::map<std::string, double> const& values)
            {
                if (values.find("a") != values.end())
                    _a = values.find("a")->second;
                else
                {
                    _a = 1;
                    std::cout << "WARNING: value 'a' not found for 'AYInferiorToEq', default used (1)\n";
                }
            }

            virtual bool Check(double, double y, double, double eq) const
            {
                return _a * y < eq;
            }
    };

}}}

#endif

