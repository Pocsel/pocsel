#ifndef __SERVER_GAME_MAP_GEN_EQUATIONS_EQUATIONPERLIN2_HPP__
#define __SERVER_GAME_MAP_GEN_EQUATIONS_EQUATIONPERLIN2_HPP__

#include "server2/game/map/gen/IEquation.hpp"

namespace Server { namespace Game { namespace Map { namespace Gen { namespace Equations {

    class EquationPerlin2 :
        public IEquation
    {
    private:
        Perlin const& _perlin;
        double xn;
        double xa;
        double zn;
        double za;
        double mul;
        double alpha;
        double beta;
        double n;

    public:
        explicit EquationPerlin2(Perlin const& perlin, std::map<std::string, double> const& vals) :
            _perlin(perlin),
            xn(0.00130),
            xa(-0.0112),
            zn(0.00139),
            za(0.0761),
            mul(123.5545),
            alpha(1.7),
            beta(2.0),
            n(8)
        {
            if (vals.find("xn") != vals.end())
                xn = vals.find("xn")->second;
            if (vals.find("xa") != vals.end())
                xa = vals.find("xa")->second;
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

        virtual double Calc(double x, double, double z) const
        {
            return _perlin.Noise2D(x * xn + xa,
                                   z * zn + za,
                                   alpha,
                                   beta,
                                   n) * mul;
        }

        virtual bool Is2D() const
        {
            return true;
        }

    };

}}}}}

#endif
