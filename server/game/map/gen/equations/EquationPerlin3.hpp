#ifndef __SERVER_GAME_MAP_GEN_EQUATIONS_EQUATIONPERLIN3_HPP__
#define __SERVER_GAME_MAP_GEN_EQUATIONS_EQUATIONPERLIN3_HPP__

#include "server/game/map/gen/IEquation.hpp"

namespace Server { namespace Game { namespace Map { namespace Gen { namespace Equations {

    class EquationPerlin3 :
        public IEquation
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
        int ip;
        int ipp;

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
            n(1),
            ip(0),
            ipp(0)
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

//        virtual double Calc(double x, double y, double z) const
//        {
//            return _perlin.Noise3D(x * xn + xa,
//                                   y * yn + ya,
//                                   z * zn + za,
//                                   alpha,
//                                   beta,
//                                   n) * mul;
//        }
//
//        virtual bool Is2D() const
//        {
//            return false;
//        }
//
//        virtual bool ByChunkCalculation() const
//        {
//            return true;
//        }

        virtual void Calc(double x, double y, double z, double* res) const
        {
            unsigned int ix, iz, iy;
            double xx, zz, yy;

            if (ipp == 0)
            {
                for (ix = 0; ix < Common::ChunkSize; ++ix)
                {
                    xx = x + (double)ix / Common::ChunkSize;
                    for (iz = 0; iz < Common::ChunkSize; ++iz)
                    {
                        zz = z + (double)iz / Common::ChunkSize;
                        for (iy = 0; iy < Common::ChunkSize; ++iy)
                        {
                            yy = y + (double)iy / Common::ChunkSize;
                            *res++ = _perlin.Noise3D(xx * xn + xa,
                                                     yy * yn + ya,
                                                     zz * zn + za,
                                                     alpha,
                                                     beta,
                                                     n) * mul;
                        }
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
                    for (iy = ipp / 2; iy < Common::ChunkSize; iy += 1 + ipp)
                    {
                        yy = y + (double)iy / Common::ChunkSize;

                        res[ix * Common::ChunkSize2 + iz * Common::ChunkSize + iy] =
                            _perlin.Noise3D(xx * xn + xa,
                                            yy * yn + ya,
                                            zz * zn + za,
                                            alpha,
                                            beta,
                                            n) * mul;
                    }
                }
            }

            // INTERPOLATION

            int x0, x1, xc;
            int z0, z1, zc;
            int y0, y1, yc;
            int ipp1 = ipp + 1;

            int xtab[Common::ChunkSize];
            int ztab[Common::ChunkSize];
            int ytab[Common::ChunkSize];

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
            SET_TAB_XZY(y);

            double* resBase = res;

            for (ix = 0; ix < Common::ChunkSize; ++ix)
            {
                xc = xtab[ix] * Common::ChunkSize2;

                for (iz = 0; iz < Common::ChunkSize; ++iz)
                {
                    zc = ztab[iz] * Common::ChunkSize + xc;

                    for (iy = 0; iy < Common::ChunkSize; ++iy)
                    {
                        yc = ytab[iy] + zc;

                        *res++ = resBase[yc];
                    }
                }
            }

        }
#undef SET_TAB_XZY
    };

}}}}}

#endif

