#ifndef __SERVER_MAPGEN_EQUATIONS_NONE_HPP__
#define __SERVER_MAPGEN_EQUATIONS_NONE_HPP__

#include "server/mapgen/Equation.hpp"

namespace Server { namespace MapGen {

    namespace Equations {

        class None1 : public Equation
        {
            public:
                explicit None1(std::map<std::string, double> const&)
                {
                }

                virtual double Calc(double, double, double) const
                {
                    return 0;
                }

                virtual bool Is2D() const
                {
                    return true;
                }
        };

    }}}

#endif

