#ifndef __SERVER_MAPGEN_VALIDATORS_YINFERIORTOEQ_HPP__
#define __SERVER_MAPGEN_VALIDATORS_YINFERIORTOEQ_HPP__

#include "server/mapgen/Validator.hpp"

namespace Server { namespace MapGen { namespace Validators {

    class YInferiorToEq : public Validator
    {
        public:
            explicit YInferiorToEq(std::map<std::string, double> const&)
            {
            }

            virtual bool Check(double, double y, double, double eq) const
            {
                return y < eq;
            }
    };

}}}

#endif
