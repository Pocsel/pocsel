#ifndef __SERVER_MAPGEN_EQUATION_HPP__
#define __SERVER_MAPGEN_EQUATION_HPP__

namespace Server { namespace MapGen {

    class Equation
    {
        public:
        virtual double Calc(double x, double y, double z) const = 0;
        virtual bool Is2D() const = 0;
    };

}}

#endif
