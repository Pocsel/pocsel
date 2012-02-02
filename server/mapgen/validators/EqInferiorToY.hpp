#ifndef __SERVER_MAPGEN_VALIDATORS_EQINFERIORTOY_HPP__
#define __SERVER_MAPGEN_VALIDATORS_EQINFERIORTOY_HPP__

#include "server/mapgen/Validator.hpp"

namespace Server { namespace MapGen { namespace Validators {

    class EqInferiorToY : public Validator
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

}}}

#endif
