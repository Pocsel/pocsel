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

        virtual void Calc(double x, double y, double z, Uint32 cx, Uint32 cy, Uint32 cz, double* res, unsigned int offset) const
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
                            *res = _perlin.Noise3D(xx * xn + xa,
                                                   yy * yn + ya,
                                                   zz * zn + za,
                                                   alpha,
                                                   beta,
                                                   n) * mul;
                            res += offset;
                        }
                    }
                }
                return;
            }


            int ipp1 = ipp + 1;

            int xDiff = -(int)(((Uint64)cx * Common::ChunkSize) % ipp1);
            int zDiff = -(int)(((Uint64)cz * Common::ChunkSize) % ipp1);
            int yDiff = -(int)(((Uint64)cy * Common::ChunkSize) % ipp1);

            unsigned int xnbCalcs = 0;
            int iix = xDiff;
            while (iix < (int)Common::ChunkSize)
            {
                ++xnbCalcs;
                if (iix < (int)Common::ChunkSize - 1 &&
                    iix + ipp1 >= (int)Common::ChunkSize)
                    ++xnbCalcs;
                iix += ipp1;
            }

            unsigned int znbCalcs = 0;
            int iiz = zDiff;
            while (iiz < (int)Common::ChunkSize)
            {
                ++znbCalcs;
                if (iiz < (int)Common::ChunkSize - 1 &&
                    iiz + ipp1 >= (int)Common::ChunkSize)
                    ++znbCalcs;
                iiz += ipp1;
            }

            unsigned int ynbCalcs = 0;
            int iiy = yDiff;
            while (iiy < (int)Common::ChunkSize)
            {
                ++ynbCalcs;
                if (iiy < (int)Common::ChunkSize - 1 &&
                    iiy + ipp1 >= (int)Common::ChunkSize)
                    ++ynbCalcs;
                iiy += ipp1;
            }


            double calcs[(Common::ChunkSize / 2 + 1) * (Common::ChunkSize / 2 + 1) * (Common::ChunkSize / 2 + 1)];
            double* calcsPtr = calcs;

            x += (double)xDiff / Common::ChunkSize;
            z += (double)zDiff / Common::ChunkSize;
            y += (double)yDiff / Common::ChunkSize;

            for (ix = 0; ix < xnbCalcs; ++ix)
            {
                xx = x + ((double)ix * ipp1) / Common::ChunkSize;

                for (iz = 0; iz < znbCalcs; ++iz)
                {
                    zz = z + ((double)iz * ipp1) / Common::ChunkSize;

                    for (iy = 0; iy < ynbCalcs; ++iy)
                    {
                        yy = y + ((double)iy * ipp1) / Common::ChunkSize;

                        *calcsPtr++ =
                            _perlin.Noise3D(xx * xn + xa,
                                            yy * yn + ya,
                                            zz * zn + za,
                                            alpha,
                                            beta,
                                            n) * mul;
                    }
                }
            }

            switch (ip)
            {
                case 0:
                    _InterpolationNearest(res, offset, calcs,
                                          xnbCalcs, znbCalcs, ynbCalcs,
                                          xDiff, zDiff, yDiff);
                    break;
                default:
                    _InterpolationLinear(res, offset, calcs,
                                         xnbCalcs, znbCalcs, ynbCalcs,
                                         xDiff, zDiff, yDiff);
            }
        }

    private:
        void _InterpolationLinear(double* res, unsigned int offset, double* calcs,
                                  unsigned int, unsigned int znbCalcs, unsigned int ynbCalcs,
                                  int xDiff, int zDiff, int yDiff
                                  ) const
        {
            unsigned int ix, iz, iy;
            double p;

            int x0, x1, x0s, x1s;
            int z0, z1, z0s, z1s;
            int y0, y1, y0s, y1s;

            int x1_0, x1_i, xi_0;
            int z1_0, z1_i, zi_0;
            int y1_0, y1_i, yi_0;
            int x1_0z1_0;
            int x1_0y1_0;
            int z1_0y1_0;
            int x1_0z1_0y1_0;

            double p000, p001, p010, p011, p100, p101, p110, p111;

            // index dans calcs
            int xctab[Common::ChunkSize * 2];
            int zctab[Common::ChunkSize * 2];
            int yctab[Common::ChunkSize * 2];

            // positions en vrai dans l'espace
            int xtab[Common::ChunkSize * 2];
            int ztab[Common::ChunkSize * 2];
            int ytab[Common::ChunkSize * 2];

            int ipp1 = ipp + 1;

            unsigned int j;
            int i0, i1;

#define INIT_CALCINDEX_TAB(t) \
            i0 = 0; \
            i1 = 1; \
            j = 0;\
            for (unsigned int i = 0; i < Common::ChunkSize; ++i) \
            { \
                if (((int)i - t##Diff) % ipp1 == 0) \
                { \
                    if (i != 0) \
                        ++j; \
                    i0 = j; \
                    i1 = j + 1; \
                    t##ctab[i * 2] = i0; \
                    t##ctab[i * 2 + 1] = i0; \
                    t##tab[i * 2] = t##Diff + i0 * ipp1; \
                    t##tab[i * 2 + 1] = t##Diff + i0 * ipp1; \
                    \
                } \
                else \
                { \
                    t##ctab[i * 2] = i0; \
                    t##ctab[i * 2 + 1] = i1; \
                    t##tab[i * 2] = t##Diff + i0 * ipp1; \
                    t##tab[i * 2 + 1] = t##Diff + i1 * ipp1; \
                } \
            }

            INIT_CALCINDEX_TAB(x);
            INIT_CALCINDEX_TAB(z);
            INIT_CALCINDEX_TAB(y);

#undef INIT_CALCINDEX_TAB

            for (ix = 0; ix < Common::ChunkSize; ++ix)
            {
                x0 = xtab[ix * 2];
                x1 = xtab[ix * 2 + 1];

                x0s = xctab[ix * 2] * ynbCalcs * znbCalcs;
                x1s = xctab[ix * 2 + 1] * ynbCalcs * znbCalcs;

                for (iz = 0; iz < Common::ChunkSize; ++iz)
                {
                    z0 = ztab[iz * 2];
                    z1 = ztab[iz * 2 + 1];

                    z0s = zctab[iz * 2] * ynbCalcs;
                    z1s = zctab[iz * 2 + 1] * ynbCalcs;

                    for (iy = 0; iy < Common::ChunkSize; ++iy)
                    {
                        y0 = ytab[iy * 2];
                        y1 = ytab[iy * 2 + 1];

                        y0s = yctab[iy * 2];
                        y1s = yctab[iy * 2 + 1];

                        if (y1 == y0)
                        {
                            if (z1 == z0)
                            {
                                if (x0 == x1)
                                {
                                    p = calcs[x0s + z0s + y0s];
                                }
                                else
                                {
                                    p000 = calcs[x0s + z0s + y0s];
                                    p100 = calcs[x1s + z0s + y0s];

                                    x1_0 = x1 - x0;
                                    x1_i = x1 - (int)ix;
                                    xi_0 = (int)ix - x0;

                                    p = (p000*x1_i)/(double)x1_0 +
                                        (p100*xi_0)/(double)x1_0;
                                }
                            }
                            else if (x0 == x1)
                            {
                                p000 = calcs[x0s + z0s + y0s];
                                p010 = calcs[x0s + z1s + y0s];

                                z1_0 = z1 - z0;
                                z1_i = z1 - (int)iz;
                                zi_0 = (int)iz - z0;

                                p = (p000*z1_i)/(double)z1_0 +
                                    (p010*zi_0)/(double)z1_0;
                            }
                            else
                            {
                                p000 = calcs[x0s + z0s + y0s];
                                p010 = calcs[x0s + z1s + y0s];
                                p100 = calcs[x1s + z0s + y0s];
                                p110 = calcs[x1s + z1s + y0s];

                                x1_0 = x1 - x0;
                                x1_i = x1 - (int)ix;
                                xi_0 = (int)ix - x0;

                                z1_0 = z1 - z0;
                                z1_i = z1 - (int)iz;
                                zi_0 = (int)iz - z0;

                                x1_0z1_0 = x1_0 * z1_0;

                                p = (p000*x1_i*z1_i)/(double)x1_0z1_0 +
                                    (p100*xi_0*z1_i)/(double)x1_0z1_0 +
                                    (p010*x1_i*zi_0)/(double)x1_0z1_0 +
                                    (p110*xi_0*zi_0)/(double)x1_0z1_0;
                            }
                        }
                        else
                        {
                            if (z1 == z0)
                            {
                                if (x0 == x1)
                                {
                                    p000 = calcs[x0s + z0s + y0s];
                                    p001 = calcs[x0s + z0s + y1s];

                                    y1_0 = y1 - y0;
                                    y1_i = y1 - (int)iy;
                                    yi_0 = (int)iy - y0;

                                    p = (
                                         (p000*y1_i) +
                                         (p001*yi_0)
                                        )/(double)y1_0;
                                }
                                else
                                {
                                    p000 = calcs[x0s + z0s + y0s];
                                    p001 = calcs[x0s + z0s + y1s];
                                    p100 = calcs[x1s + z0s + y0s];
                                    p101 = calcs[x1s + z0s + y1s];

                                    x1_0 = x1 - x0;
                                    x1_i = x1 - (int)ix;
                                    xi_0 = (int)ix - x0;

                                    y1_0 = y1 - y0;
                                    y1_i = y1 - (int)iy;
                                    yi_0 = (int)iy - y0;

                                    x1_0y1_0 = x1_0 * y1_0;

                                    p = (
                                         (p000*x1_i*y1_i) +
                                         (p001*x1_i*yi_0) +
                                         (p100*xi_0*y1_i) +
                                         (p101*xi_0*yi_0)
                                        )/(double)x1_0y1_0;
                                }
                            }
                            else if (x0 == x1)
                            {
                                p000 = calcs[x0s + z0s + y0s];
                                p001 = calcs[x0s + z0s + y1s];
                                p010 = calcs[x0s + z1s + y0s];
                                p011 = calcs[x0s + z1s + y1s];

                                z1_0 = z1 - z0;
                                z1_i = z1 - (int)iz;
                                zi_0 = (int)iz - z0;

                                y1_0 = y1 - y0;
                                y1_i = y1 - (int)iy;
                                yi_0 = (int)iy - y0;

                                z1_0y1_0 = z1_0 * y1_0;

                                p = (
                                     (p000*z1_i*y1_i) +
                                     (p001*z1_i*yi_0) +
                                     (p010*zi_0*y1_i) +
                                     (p011*zi_0*yi_0)
                                    )/(double)z1_0y1_0;
                            }
                            else
                            {
                                p000 = calcs[x0s + z0s + y0s];
                                p001 = calcs[x0s + z0s + y1s];
                                p010 = calcs[x0s + z1s + y0s];
                                p011 = calcs[x0s + z1s + y1s];
                                p100 = calcs[x1s + z0s + y0s];
                                p101 = calcs[x1s + z0s + y1s];
                                p110 = calcs[x1s + z1s + y0s];
                                p111 = calcs[x1s + z1s + y1s];

                                x1_0 = x1 - x0;
                                x1_i = x1 - (int)ix;
                                xi_0 = (int)ix - x0;

                                z1_0 = z1 - z0;
                                z1_i = z1 - (int)iz;
                                zi_0 = (int)iz - z0;

                                y1_0 = y1 - y0;
                                y1_i = y1 - (int)iy;
                                yi_0 = (int)iy - y0;

                                x1_0z1_0y1_0 = x1_0 * z1_0 * y1_0;

                                p = (
                                     (p000*x1_i*z1_i*y1_i) +
                                     (p001*x1_i*z1_i*yi_0) +
                                     (p010*x1_i*zi_0*y1_i) +
                                     (p011*x1_i*zi_0*yi_0) +
                                     (p100*xi_0*z1_i*y1_i) +
                                     (p101*xi_0*z1_i*yi_0) +
                                     (p110*xi_0*zi_0*y1_i) +
                                     (p111*xi_0*zi_0*yi_0)
                                    )/(double)x1_0z1_0y1_0;
                            }
                        }

                        *res = p;
                        res += offset;
                    }
                }
            }
        }

        void _InterpolationNearest(double* res, unsigned int offset, double* calcs,
                                   unsigned int, unsigned int znbCalcs, unsigned int ynbCalcs,
                                   int xDiff, int zDiff, int yDiff
                                  ) const
        {
            unsigned int ix, iz, iy;

            int xc;
            int zc;
            int yc;

            int xtab[Common::ChunkSize];
            int ztab[Common::ChunkSize];
            int ytab[Common::ChunkSize];

            int ipp1 = ipp + 1;

            unsigned int j;
            int i0, i1;

#define INIT_CALCINDEX_TAB(t) \
            i0 = 0; \
            i1 = 1; \
            j = 0;\
            for (unsigned int i = 0; i < Common::ChunkSize; ++i) \
            { \
                if (((int)i - t##Diff) % ipp1 == 0) \
                { \
                    if (i != 0) \
                        ++j; \
                    i0 = j; \
                    i1 = j + 1; \
                    t##tab[i] = i0; \
                } \
                else \
                { \
                    int reali = (int)i - t##Diff; \
                    if (reali < 0) \
                        reali = ipp1-reali; \
                    if (reali % ipp1 < ipp1 / 2) \
                        t##tab[i] = i0; \
                    else \
                        t##tab[i] = i1; \
                } \
            }

            INIT_CALCINDEX_TAB(x);
            INIT_CALCINDEX_TAB(z);
            INIT_CALCINDEX_TAB(y);

#undef INIT_CALCINDEX_TAB

            for (ix = 0; ix < Common::ChunkSize; ++ix)
            {
                xc = xtab[ix] * ynbCalcs * znbCalcs;

                for (iz = 0; iz < Common::ChunkSize; ++iz)
                {
                    zc = ztab[iz] * ynbCalcs + xc;

                    for (iy = 0; iy < Common::ChunkSize; ++iy)
                    {
                        yc = ytab[iy] + zc;

                        *res = calcs[yc];
                        res += offset;
                    }
                }
            }

        }
    };

}}}}}

#endif

