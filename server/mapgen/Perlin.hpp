#ifndef __SERVER_MAPGEN_PERLIN_HPP__
# define __SERVER_MAPGEN_PERLIN_HPP__

namespace Server
{
    namespace MapGen
    {

        class IRand;

        class Perlin
        {
        private:
            static int const _B = 0x100;
            static int const _BM = 0xff;
            static int const _N = 0x1000;
            static int const _NP = 12; // 2^N
            static int const _NM = 0xfff;

            // static Int32 _p[_B + _B + 2]; // 320ms en static vs 350ms
            // static double _g3[_B + _B + 2][3];
            // static double _g2[_B + _B + 2][2];
            // static double _g1[_B + _B + 2];

            Int32 _p[_B + _B + 2];
            double _g3[_B + _B + 2][3];
            double _g2[_B + _B + 2][2];
            double _g1[_B + _B + 2];

        public:
            explicit Perlin(IRand& random_algo);
            ~Perlin();
            double Noise1D(double x, double weight, double scale, int n) const;
            double Noise2D(double x, double y, double weight, double scale, int n) const;
            double Noise3D(double x, double y, double z, double weight, double scale, int n) const;
        private:
            double _Noise1(double vec) const;
            double _Noise2(double* vec) const;
            double _Noise3(double* vec) const;
            static void _Normalize3(double* vec);
            static void _Normalize2(double* vec);

            static inline double _SCurve(double t)
            {
                return t * t * (3.0 - 2.0 * t);
            }
            static inline double _Lerp(double t, double a, double b)
            {
                return a + t * (b - a);
            }
            static inline void _Setup(int i, int& b0, int& b1, double& r0, double& r1, double* vec, double& t)
            {
                t = vec[i] + _N;
                b0 = ((int)t) & _BM;
                b1 = (b0 + 1) & _BM;
                r0 = t - (int)t;
                r1 = r0 - 1.;
            }
            static inline double _At2(double rx, double ry, double const* q)
            {
                return rx * q[0] + ry * q[1];
            }
            static inline double _At3(double rx, double ry, double rz, double const* q)
            {
                return rx * q[0] + ry * q[1] + rz * q[2];
            }
        };

    }
}

#endif
