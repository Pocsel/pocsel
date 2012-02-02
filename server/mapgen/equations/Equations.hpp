#ifndef __SERVER_MAPGEN_EQUATIONS_EQUATIONS_HPP__
#define __SERVER_MAPGEN_EQUATIONS_EQUATIONS_HPP__

#include "server/mapgen/equations/EquationPerlin2.hpp"
#include "server/mapgen/equations/EquationPerlin3.hpp"
#include "server/mapgen/equations/None.hpp"
#include "tools/CaseInsensitive.hpp"

namespace Server { namespace MapGen {

    class Perlin;

    namespace Equations {

        class Equations
        {
            private:
                std::map<std::string, Equation* (Equations::*)(std::map<std::string, double> const&), Tools::CaseInsensitive> _equationCreators;
                Perlin const& _perlin;

            public:

                Equation* CreateEquation(std::string const& name, std::map<std::string, double> const& values)
                {
                    if (this->_equationCreators.find(name) == this->_equationCreators.end())
                        return 0;
                    return (this->*(this->_equationCreators[name]))(values);
                }

#define CREATE_THIS_ONE_PERLIN(eq) Equation* _Create##eq(std::map<std::string, double> const& values) \
                { \
                    return new eq(this->_perlin, values); \
                } \

                CREATE_THIS_ONE_PERLIN(EquationPerlin2)
                CREATE_THIS_ONE_PERLIN(EquationPerlin3)

#undef CREATE_THIS_ONE_PERLIN

#define CREATE_THIS_ONE(eq) Equation* _Create##eq(std::map<std::string, double> const& values) \
                { \
                    return new eq(values); \
                } \

                CREATE_THIS_ONE(None1)

#undef CREATE_THIS_ONE

#define ADD_THIS_ONE(str, eq) this->_equationCreators[str] = &Equations::_Create##eq;
                    Equations(Perlin const& perlin) :
                        _perlin(perlin)
                {
                    ADD_THIS_ONE("perlin2", EquationPerlin2);
                    ADD_THIS_ONE("perlin2d", EquationPerlin2);

                    ADD_THIS_ONE("perlin3", EquationPerlin3);
                    ADD_THIS_ONE("perlin3d", EquationPerlin3);

                    ADD_THIS_ONE("none", None1);
                    ADD_THIS_ONE("zero", None1);
                    ADD_THIS_ONE("empty", None1);
                    ADD_THIS_ONE("void", None1);
                    ADD_THIS_ONE("nothing", None1);
                }
#undef ADD_THIS_ONE



            private:
        };

    }}}

#endif
