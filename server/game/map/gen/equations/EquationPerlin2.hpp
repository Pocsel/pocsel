#ifndef __SERVER_GAME_MAP_GEN_EQUATIONS_EQUATIONPERLIN2_HPP__
#define __SERVER_GAME_MAP_GEN_EQUATIONS_EQUATIONPERLIN2_HPP__

#include "server/game/map/gen/IEquation.hpp"

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
        int ip;
        int ipp;

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
            n(8),
            ip(0),
            ipp(0)
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
            if (vals.find("ip") != vals.end())
            {
                ip = vals.find("ip")->second + 0.1;
                if (ip < 0)
                    ip = 0;
            }
            if (vals.find("ipp") != vals.end())
            {
                ipp = vals.find("ipp")->second + 0.1;
                if (ipp < 0)
                    ipp = 0;
            }
        }

        //        virtual double Calc(double x, double, double z) const
        //        {
        //            return _perlin.Noise2D(x * xn + xa,
        //                                   z * zn + za,
        //                                   alpha,
        //                                   beta,
        //                                   n) * mul;
        //        }
        //
        //        virtual bool Is2D() const
        //        {
        //            return true;
        //        }
        //
        //        virtual bool ByChunkCalculation() const
        //        {
        //            return true;
        //        }

        virtual void Calc(double x, double, double z, double* res) const
        {
            unsigned int ix, iz, iy;
            double xx, zz, p;

            if (ipp == 0)
            {
                for (ix = 0; ix < Common::ChunkSize; ++ix)
                {
                    xx = x + (double)ix / Common::ChunkSize;
                    for (iz = 0; iz < Common::ChunkSize; ++iz)
                    {
                        zz = z + (double)iz / Common::ChunkSize;
                        p = _perlin.Noise2D(xx * xn + xa,
                                            zz * zn + za,
                                            alpha,
                                            beta,
                                            n) * mul;

                        for (iy = 0; iy < Common::ChunkSize; ++iy)
                            *res++ = p;
                    }
                }
                return;
            }

            for (ix = ipp / 2; ix < Common::ChunkSize; ix += 1 + ipp)
            {
                xx = x + (double)ix / Common::ChunkSize;
                for (iz = ipp / 2; iz < Common::ChunkSize; iz += 1 + ipp)
                {
                    zz = z + (double)iz / Common::ChunkSize;
                    p = _perlin.Noise2D(xx * xn + xa,
                                        zz * zn + za,
                                        alpha,
                                        beta,
                                        n) * mul;
                    res[ix * Common::ChunkSize2 + iz * Common::ChunkSize] = p;
                }
            }

            // INTERPOLATION

            int x0, x1, xc;
            int z0, z1, zc;
            int ipp1 = ipp + 1;

            int xtab[Common::ChunkSize];
            int ztab[Common::ChunkSize];

#define SET_TAB_XZY(x) \
            do { \
                x##0 = ipp / 2; \
                x##1 = ipp / 2; \
                for (unsigned int i = 0; i < Common::ChunkSize; ++i) \
                { \
                    if ((int)i % ipp1 == ipp / 2) \
                    { \
                        x##0 = i; \
                        if (i + ipp1 >= Common::ChunkSize) \
                            x##1 = i; \
                        else \
                            x##1 = i + ipp1; \
                        x##tab[i] = x##0; \
                    } \
                    else \
                    { \
                        if ((int)i % ipp1 < ipp1 / 2) \
                            x##tab[i] = x##1; \
                        else \
                            x##tab[i] = x##0; \
                    } \
                } \
            } while (0);

            SET_TAB_XZY(x);
            SET_TAB_XZY(z);

            double* resBase = res;

            for (ix = 0; ix < Common::ChunkSize; ++ix)
            {
                xc = xtab[ix] * Common::ChunkSize2;

                for (iz = 0; iz < Common::ChunkSize; ++iz)
                {
                    zc = ztab[iz] * Common::ChunkSize + xc;

                    p = resBase[zc];

                    for (iy = 0; iy < Common::ChunkSize; ++iy)
                        *res++ = p;
                }
            }

        }
#undef SET_TAB_XZY
    };

}}}}}

#endif
