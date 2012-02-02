#ifndef __SERVER_MAPGEN_VALIDATOR_HPP__
#define __SERVER_MAPGEN_VALIDATOR_HPP__

namespace Server { namespace MapGen {

    class Validator
    {
        public:
            virtual bool Check(double x, double y, double z, double eq) const = 0;
    };

}}

#endif
