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

        virtual void Calc(double x, double, double z, Uint32 cx, Uint32, Uint32 cz, double* res) const
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


            int ipp1 = ipp + 1;


            unsigned int nbCalcs = 0;

            ix = 0;
            while (ix < Common::ChunkSize)
            {
                xx = x + (double)ix / Common::ChunkSize;

                ++nbCalcs;

                if (ix + ipp1 >= Common::ChunkSize && ix != Common::ChunkSize - 1)
                    ix = Common::ChunkSize - 1;
                else
                    ix += ipp1;
            }

            double calcs[(Common::ChunkSize / 2)*(Common::ChunkSize / 2)];
            double* calcsPtr = calcs;


            for (ix = 0; ix < nbCalcs; ++ix)
            {
                xx = x + (-(double)( ((Uint64)cx * Common::ChunkSize % ipp1))) / Common::ChunkSize + ((double)ix * ipp1) / Common::ChunkSize;

                for (iz = 0; iz < nbCalcs; ++iz)
                {
                    zz = z + (-(double)(((Uint64)cz * Common::ChunkSize % ipp1))) / Common::ChunkSize + ((double)iz * ipp1) / Common::ChunkSize;

                    p = _perlin.Noise2D(xx * xn + xa,
                                        zz * zn + za,
                                        alpha,
                                        beta,
                                        n) * mul;

                    *calcsPtr++ = p;
                }
            }

            switch (ip)
            {
                case 0:
                    _InterpolationNearest(res, calcs, nbCalcs);
                    break;
                default:
                    _InterpolationLinear(res, calcs, nbCalcs);
            }
        }

    private:
        void _InterpolationLinear(double* res, double* calcs, unsigned int nbCalcs) const
        {
            unsigned int ix, iz, iy;
            double p;

            int x0, x1, x0s, x1s;
            int z0, z1, z0s, z1s;

            int x1_0, x1_i, xi_0;
            int z1_0, z1_i, zi_0;
            int x1_0z1_0;

            double p00, p01, p10, p11;

            int tab[Common::ChunkSize * 2];

            int ipp1 = ipp + 1;

            x0 = 0;
            x1 = 0;
            for (unsigned int i = 0; i < Common::ChunkSize; ++i)
            {
                if ((int)i == Common::ChunkSize - 1)
                {
                    tab[i * 2] = i;
                    tab[i * 2 + 1] = i;
                }
                else if ((int)i % ipp1 == 0)
                {
                    x0 = i;
                    if (i + ipp1 >= Common::ChunkSize)
                        x1 = Common::ChunkSize - 1;
                    else
                        x1 = i + ipp1;
                    tab[i * 2] = x0;
                    tab[i * 2 + 1] = x0;
                }
                else
                {
                    tab[i * 2] = x0;
                    tab[i * 2 + 1] = x1;
                }
            }

            double* resBase = res;

            for (ix = 0; ix < Common::ChunkSize; ++ix)
            {
                x0 = tab[ix * 2];
                x1 = tab[ix * 2 + 1];

                x0s = x0 * Common::ChunkSize2;
                x1s = x1 * Common::ChunkSize2;

                for (iz = 0; iz < Common::ChunkSize; ++iz)
                {
                    z0 = tab[iz * 2];
                    z1 = tab[iz * 2 + 1];

                    if ((z1 == z0))
                    {
                        if (x0 == x1)
                        {
                            p = resBase[ix * Common::ChunkSize2 + iz * Common::ChunkSize];
                        }
                        else
                        {
                            z0s = z0 * Common::ChunkSize;
                            z1s = z1 * Common::ChunkSize;

                            p00 = resBase[x0s + z0s];
                            p10 = resBase[x1s + z0s];

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
                        z0s = z0 * Common::ChunkSize;
                        z1s = z1 * Common::ChunkSize;

                        p00 = resBase[x0s + z0s];
                        p10 = resBase[x0s + z1s];

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
                        z0s = z0 * Common::ChunkSize;
                        z1s = z1 * Common::ChunkSize;

                        p00 = resBase[x0s + z0s];
                        p01 = resBase[x0s + z1s];
                        p10 = resBase[x1s + z0s];
                        p11 = resBase[x1s + z1s];

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
                        *res++ = p;
                }
            }
        }

        void _InterpolationNearest(double* res, double* calcs, unsigned int nbCalcs) const
        {
            unsigned int ix, iz, iy;
            double p;

            int xc;
            int zc;

            int tab[Common::ChunkSize];

            int ipp1 = ipp + 1;

            int i0, i1;
            i0 = 0;
            i1 = 0;
            for (unsigned int i = 0; i < Common::ChunkSize; ++i)
            {
                if ((int)i % ipp1 == 0)
                {
                    i0 = i / ipp1;
                    if (i + ipp1 >= Common::ChunkSize)
                        i1 = nbCalcs - 1;
                    else
                        i1 = i / ipp1 + 1;
                    tab[i] = i0;
                }
                else
                {
                    if ((int)i % ipp1 < ipp1 / 2)
                        tab[i] = i0;
                    else
                        tab[i] = i1;
                }
            }

            for (ix = 0; ix < Common::ChunkSize; ++ix)
            {
                xc = tab[ix] * nbCalcs;

                for (iz = 0; iz < Common::ChunkSize; ++iz)
                {
                    zc = tab[iz] + xc;

                    p = calcs[zc];

                    for (iy = 0; iy < Common::ChunkSize; ++iy)
                        *res++ = p;
                }
            }
        }

    };

}}}}}

#endif
