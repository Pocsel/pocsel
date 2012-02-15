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

            int ipp1 = ipp + 1;

            ix = 0;
            while (ix < Common::ChunkSize)
            {
                xx = x + (double)ix / Common::ChunkSize;

                iz = 0;
                while (iz < Common::ChunkSize)
                {
                    zz = z + (double)iz / Common::ChunkSize;

                    iy = 0;
                    while (iy < Common::ChunkSize)
                    {
                        yy = y + (double)iy / Common::ChunkSize;

                        res[ix * Common::ChunkSize2 + iz * Common::ChunkSize + iy] =
                            _perlin.Noise3D(xx * xn + xa,
                                            yy * yn + ya,
                                            zz * zn + za,
                                            alpha,
                                            beta,
                                            n) * mul;

                        if (iy + ipp1 >= Common::ChunkSize && iy != Common::ChunkSize - 1)
                            iy = Common::ChunkSize - 1;
                        else
                            iy += ipp1;
                    }
                    if (iz + ipp1 >= Common::ChunkSize && iz != Common::ChunkSize - 1)
                        iz = Common::ChunkSize - 1;
                    else
                        iz += ipp1;
                }
                if (ix + ipp1 >= Common::ChunkSize && ix != Common::ChunkSize - 1)
                    ix = Common::ChunkSize - 1;
                else
                    ix += ipp1;
            }

            switch (ip)
            {
                case 0:
                    _InterpolationNearest(res);
                    break;
                default:
                    _InterpolationLinear(res);
            }
        }

    private:
        void _InterpolationLinear(double* res) const
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

                    z0s = z0 * Common::ChunkSize;
                    z1s = z1 * Common::ChunkSize;

                    for (iy = 0; iy < Common::ChunkSize; ++iy)
                    {
                        y0 = tab[iy * 2];
                        y1 = tab[iy * 2 + 1];

                        y0s = y0;
                        y1s = y1;

                        if (y1 == y0)
                        {
                            if (z1 == z0)
                            {
                                if (x0 == x1)
                                {
                                    p = resBase[ix * Common::ChunkSize2 + iz * Common::ChunkSize + iy];
                                }
                                else
                                {
                                    p000 = resBase[x0s + z0s + y0s];
                                    p100 = resBase[x1s + z0s + y0s];

                                    x1_0 = x1 - x0;
                                    x1_i = x1 - (int)ix;
                                    xi_0 = (int)ix - x0;

                                    p = (p000*x1_i)/(double)x1_0 +
                                        (p100*xi_0)/(double)x1_0;
                                }
                            }
                            else if (x0 == x1)
                            {
                                p000 = resBase[x0s + z0s + y0s];
                                p010 = resBase[x0s + z1s + y0s];

                                z1_0 = z1 - z0;
                                z1_i = z1 - (int)iz;
                                zi_0 = (int)iz - z0;

                                p = (p000*z1_i)/(double)z1_0 +
                                    (p010*zi_0)/(double)z1_0;
                            }
                            else
                            {
                                p000 = resBase[x0s + z0s + y0s];
                                p010 = resBase[x0s + z1s + y0s];
                                p100 = resBase[x1s + z0s + y0s];
                                p110 = resBase[x1s + z1s + y0s];

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
                                    p000 = resBase[x0s + z0s + y0s];
                                    p001 = resBase[x0s + z0s + y1s];

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
                                    p000 = resBase[x0s + z0s + y0s];
                                    p001 = resBase[x0s + z0s + y1s];
                                    p100 = resBase[x1s + z0s + y0s];
                                    p101 = resBase[x1s + z0s + y1s];

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
                                p000 = resBase[x0s + z0s + y0s];
                                p001 = resBase[x0s + z0s + y1s];
                                p010 = resBase[x0s + z1s + y0s];
                                p011 = resBase[x0s + z1s + y1s];

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
                                p000 = resBase[x0s + z0s + y0s];
                                p001 = resBase[x0s + z0s + y1s];
                                p010 = resBase[x0s + z1s + y0s];
                                p011 = resBase[x0s + z1s + y1s];
                                p100 = resBase[x1s + z0s + y0s];
                                p101 = resBase[x1s + z0s + y1s];
                                p110 = resBase[x1s + z1s + y0s];
                                p111 = resBase[x1s + z1s + y1s];

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

                        *res++ = p;
                    }
                }
            }
        }
        void _InterpolationNearest(double* res) const
        {
            unsigned int ix, iz, iy;

            int xc;
            int zc;
            int yc;

            int tab[Common::ChunkSize];

            int ipp1 = ipp + 1;

            int i0, i1;
            i0 = 0;
            i1 = 0;
            for (unsigned int i = 0; i < Common::ChunkSize; ++i)
            {
                if ((int)i % ipp1 == 0)
                {
                    i0 = i;
                    if (i + ipp1 >= Common::ChunkSize)
                        i1 = Common::ChunkSize - 1;
                    else
                        i1 = i + ipp1;
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

            double* resBase = res;

            for (ix = 0; ix < Common::ChunkSize; ++ix)
            {
                xc = tab[ix] * Common::ChunkSize2;

                for (iz = 0; iz < Common::ChunkSize; ++iz)
                {
                    zc = tab[iz] * Common::ChunkSize + xc;

                    for (iy = 0; iy < Common::ChunkSize; ++iy)
                    {
                        yc = tab[iy] + zc;

                        *res++ = resBase[yc];
                    }
                }
            }

        }
    };

}}}}}

#endif

