#ifndef __SERVER_MAPGEN_EQUATIONS_EQUATIONPERLIN3_HPP__
#define __SERVER_MAPGEN_EQUATIONS_EQUATIONPERLIN3_HPP__

#include "server/mapgen/Equation.hpp"

namespace Server { namespace MapGen {

    namespace Equations {

        class EquationPerlin3 : public Equation
        {
            private:
                Perlin const& _perlin;
                double xn;
                double xa;
                double yn;
                double ya;
                double zn;
                double za;
                double mul;
                double alpha;
                double beta;
                double n;

            public:
                explicit EquationPerlin3(Perlin const& perlin, std::map<std::string, double> const& vals) :
                    _perlin(perlin),
                    xn(0.063),
                    xa(-0.0937),
                    yn(0.142),
                    ya(0.0841),
                    zn(0.063),
                    za(0.0764),
                    mul(1.0),
                    alpha(2.0),
                    beta(2.0),
                    n(1)
            {
                if (vals.find("xn") != vals.end())
                    xn = vals.find("xn")->second;
                if (vals.find("xa") != vals.end())
                    xa = vals.find("xa")->second;
                if (vals.find("yn") != vals.end())
                    yn = vals.find("yn")->second;
                if (vals.find("ya") != vals.end())
                    ya = vals.find("ya")->second;
                if (vals.find("zn") != vals.end())
                    zn = vals.find("zn")->second;
                if (vals.find("za") != vals.end())
                    za = vals.find("za")->second;
                if (vals.find("mul") != vals.end())
                    mul = vals.find("mul")->second;
                if (vals.find("alpha") != vals.end())
                    alpha = vals.find("alpha")->second;
                if (vals.find("beta") != vals.end())
                    beta = vals.find("beta")->second;
                if (vals.find("n") != vals.end())
                    n = vals.find("n")->second;
            }

                virtual double Calc(double x, double y, double z) const
                {
                    return _perlin.Noise3D(x * xn + xa,
                            y * yn + ya,
                            z * zn + za,
                            alpha,
                            beta,
                            n) * mul;
                }

                virtual bool Is2D() const
                {
                    return false;
                }


        };

    }}}

#endif

