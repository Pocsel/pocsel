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
        int ippmx;
        int ippmz;

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
                ip = (int)vals.find("ip")->second;
                if (ip < 0)
                    ip = 0;
            }
            if (vals.find("ipp") != vals.end())
            {
                ipp = (int)vals.find("ipp")->second;
                if (ipp < 0)
                    ipp = 0;
            }
            if (vals.find("ippmx") != vals.end())
            {
                ippmx = (int)vals.find("ippmx")->second;
                if (ippmx < 1)
                    ippmx = 1;
            }
            if (vals.find("ippmz") != vals.end())
            {
                ippmz = (int)vals.find("ippmz")->second;
                if (ippmz < 1)
                    ippmz = 1;
            }
        }

        virtual void Calc(double x, double, double z, Uint32 cx, Uint32, Uint32 cz, double* res, unsigned int offset) const
        {
            unsigned int ix, iz, iy;
            double xx, zz, p;

            if (ipp == 0)
            {
                for (ix = 0; ix < Common::ChunkSize; ++ix)
                {
                    xx = x + (double)ix;// / Common::ChunkSize;
                    for (iz = 0; iz < Common::ChunkSize; ++iz)
                    {
                        zz = z + (double)iz;// / Common::ChunkSize;
                        p = _perlin.Noise2D(xx * xn + xa,
                                            zz * zn + za,
                                            alpha,
                                            beta,
                                            (int)n) * mul;

                        for (iy = 0; iy < Common::ChunkSize; ++iy)
                        {
                            *res = p;
                            res += offset;
                        }
                    }
                }
                return;
            }


#define INIT_XDIFF_XNBCALCS(t) \
            int ipp##t = ipp * ippm##t; \
            int ipp1##t = ipp##t + 1; \
            int t##Diff = -(int)(((Uint64)c##t * Common::ChunkSize) % ipp1##t); \
            unsigned int t##nbCalcs = 0; \
            int ii##t = t##Diff; \
            while (ii##t < (int)Common::ChunkSize) \
            { \
                ++t##nbCalcs; \
                if (ii##t < (int)Common::ChunkSize - 1 && \
                    ii##t + ipp1##t >= (int)Common::ChunkSize) \
                    ++t##nbCalcs; \
                ii##t += ipp1##t; \
            }

            INIT_XDIFF_XNBCALCS(x);
            INIT_XDIFF_XNBCALCS(z);

#undef INIT_XDIFF_XNBCALCS

            double calcs[(Common::ChunkSize / 2 + 1) * (Common::ChunkSize / 2 + 1)];
            double* calcsPtr = calcs;

            x += (double)xDiff;// / Common::ChunkSize;
            z += (double)zDiff;// / Common::ChunkSize;

            for (ix = 0; ix < xnbCalcs; ++ix)
            {
                xx = x + ((double)ix * ipp1x);// / Common::ChunkSize;

                for (iz = 0; iz < znbCalcs; ++iz)
                {
                    zz = z + ((double)iz * ipp1z);// / Common::ChunkSize;

                    p = _perlin.Noise2D(xx * xn + xa,
                                        zz * zn + za,
                                        alpha,
                                        beta,
                                        (int)n) * mul;

                    *calcsPtr++ = p;
                }
            }

            switch (ip)
            {
                case 0:
                    _InterpolationNearest(res, offset, calcs, xnbCalcs, znbCalcs, xDiff, zDiff);
                    break;
                default:
                    _InterpolationLinear(res, offset, calcs, xnbCalcs, znbCalcs, xDiff, zDiff);
            }

        }

    private:
        void _InterpolationLinear(double* res, unsigned int offset, double* calcs,
                                  unsigned int, unsigned int znbCalcs,
                                  int xDiff, int zDiff) const
        {
            unsigned int ix, iz, iy;
            double p;

            int x0, x1, x0s, x1s;
            int z0, z1, z0s, z1s;

            int x1_0, x1_i, xi_0;
            int z1_0, z1_i, zi_0;
            int x1_0z1_0;

            double p00, p01, p10, p11;

            // index dans calcs
            int xctab[Common::ChunkSize * 2];
            int zctab[Common::ChunkSize * 2];

            // positions en vrai dans l'espace
            int xtab[Common::ChunkSize * 2];
            int ztab[Common::ChunkSize * 2];

            unsigned int j;
            int i0, i1;

#define INIT_CALCINDEX_TAB(t) \
            int ipp##t = ipp * ippm##t; \
            int ipp1##t = ipp##t + 1; \
            i0 = 0; \
            i1 = 1; \
            j = 0;\
            for (unsigned int i = 0; i < Common::ChunkSize; ++i) \
            { \
                if (((int)i - t##Diff) % ipp1##t == 0) \
                { \
                    if (i != 0) \
                        ++j; \
                    i0 = j; \
                    i1 = j + 1; \
                    t##ctab[i * 2] = i0; \
                    t##ctab[i * 2 + 1] = i0; \
                    t##tab[i * 2] = t##Diff + i0 * ipp1##t; \
                    t##tab[i * 2 + 1] = t##Diff + i0 * ipp1##t; \
                    \
                } \
                else \
                { \
                    t##ctab[i * 2] = i0; \
                    t##ctab[i * 2 + 1] = i1; \
                    t##tab[i * 2] = t##Diff + i0 * ipp1##t; \
                    t##tab[i * 2 + 1] = t##Diff + i1 * ipp1##t; \
                } \
            }

            INIT_CALCINDEX_TAB(x);
            INIT_CALCINDEX_TAB(z);

#undef INIT_CALCINDEX_TAB

            for (ix = 0; ix < Common::ChunkSize; ++ix)
            {
                x0 = xtab[ix * 2];
                x1 = xtab[ix * 2 + 1];

                x0s = xctab[ix * 2] * znbCalcs;
                x1s = xctab[ix * 2 + 1] * znbCalcs;

                for (iz = 0; iz < Common::ChunkSize; ++iz)
                {
                    z0 = ztab[iz * 2];
                    z1 = ztab[iz * 2 + 1];

                    z0s = zctab[iz * 2];
                    z1s = zctab[iz * 2] + 1;

                    if ((z1 == z0))
                    {
                        if (x0 == x1)
                        {
                            p = calcs[x0s + z0s];
                        }
                        else
                        {
                            p00 = calcs[x0s + z0s];
                            p10 = calcs[x1s + z0s];

                            x1_0 = x1 - x0;
                            x1_i = x1 - (int)ix;
                            xi_0 = (int)ix - x0;

                            p = (
                                 (p00*x1_i) +
                                 (p10*xi_0)
                                )/(double)x1_0;
                        }
                    }
                    else if (x0 == x1)
                    {
                        p00 = calcs[x0s + z0s];
                        p10 = calcs[x0s + z1s];

                        z1_0 = z1 - z0;
                        z1_i = z1 - (int)iz;
                        zi_0 = (int)iz - z0;

                        p = (
                             (p00*z1_i) +
                             (p10*zi_0)
                            )/(double)z1_0;
                    }
                    else
                    {
                        p00 = calcs[x0s + z0s];
                        p01 = calcs[x0s + z1s];
                        p10 = calcs[x1s + z0s];
                        p11 = calcs[x1s + z1s];

                        x1_0 = x1 - x0;
                        x1_i = x1 - (int)ix;
                        xi_0 = (int)ix - x0;

                        z1_0 = z1 - z0;
                        z1_i = z1 - (int)iz;
                        zi_0 = (int)iz - z0;

                        x1_0z1_0 = x1_0 * z1_0;

                        p = (
                             (p00*x1_i*z1_i) +
                             (p10*xi_0*z1_i) +
                             (p01*x1_i*zi_0) +
                             (p11*xi_0*zi_0)
                            )/(double)x1_0z1_0;
                    }

                    for (iy = 0; iy < Common::ChunkSize; ++iy)
                    {
                        *res = p;
                        res += offset;
                    }
                }
            }
        }

        void _InterpolationNearest(double* res, unsigned int offset, double* calcs,
                                   unsigned int, unsigned int znbCalcs,
                                   int xDiff, int zDiff) const
        {
            unsigned int ix, iz, iy;
            double p;

            int xc;
            int zc;

            int xtab[Common::ChunkSize];
            int ztab[Common::ChunkSize];

            unsigned int j;
            int i0, i1;

#define INIT_CALCINDEX_TAB(t) \
            int ipp##t = ipp * ippm##t; \
            int ipp1##t = ipp##t + 1; \
            i0 = 0; \
            i1 = 1; \
            j = 0;\
            for (unsigned int i = 0; i < Common::ChunkSize; ++i) \
            { \
                if (((int)i - t##Diff) % ipp1##t == 0) \
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
                        reali = ipp1##t-reali; \
                    if (reali % ipp1##t < ipp1##t / 2) \
                        t##tab[i] = i0; \
                    else \
                        t##tab[i] = i1; \
                } \
            }

            INIT_CALCINDEX_TAB(x);
            INIT_CALCINDEX_TAB(z);

#undef INIT_CALCINDEX_TAB

            for (ix = 0; ix < Common::ChunkSize; ++ix)
            {
                xc = xtab[ix] * znbCalcs;

                for (iz = 0; iz < Common::ChunkSize; ++iz)
                {
                    zc = ztab[iz] + xc;

                    p = calcs[zc];

                    for (iy = 0; iy < Common::ChunkSize; ++iy)
                    {
                        *res = p;
                        res += offset;
                    }
                }
            }
        }

    };

}}}}}

#endif
