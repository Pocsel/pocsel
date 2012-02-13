#include "server2/game/map/gen/Perlin.hpp"
#include "server2/game/map/gen/IRand.hpp"

namespace Server { namespace Game { namespace Map { namespace Gen {

    // Int32  Perlin::_p[_B + _B + 2]; // 320ms en static vs 350ms
    // double Perlin::_g3[_B + _B + 2][3];
    // double Perlin::_g2[_B + _B + 2][2];
    // double Perlin::_g1[_B + _B + 2];
    Perlin::Perlin(IRand& random_algo)
    {
        int i, j, k;

        for (i = 0 ; i < _B ; i++)
        {
            _p[i] = i;
            _g1[i] = (double)((random_algo.Rand() % (Perlin::_B + Perlin::_B)) - Perlin::_B) / Perlin::_B;

            for (j = 0 ; j < 2 ; j++)
                _g2[i][j] = (double)((random_algo.Rand() % (Perlin::_B + Perlin::_B)) - Perlin::_B) / Perlin::_B;
            Perlin::_Normalize2(_g2[i]);

            for (j = 0 ; j < 3 ; j++)
                _g3[i][j] = (double)((random_algo.Rand() % (Perlin::_B + Perlin::_B)) - Perlin::_B) / Perlin::_B;
            Perlin::_Normalize3(_g3[i]);
        }

        while (--i)
        {
            k = _p[i];
            _p[i] = _p[j = random_algo.Rand() % Perlin::_B];
            _p[j] = k;
        }

        for (i = 0 ; i < Perlin::_B + 2 ; i++)
        {
            _p[Perlin::_B + i] = _p[i];
            _g1[Perlin::_B + i] = _g1[i];
            for (j = 0 ; j < 2 ; j++)
                _g2[Perlin::_B + i][j] = _g2[i][j];
            for (j = 0 ; j < 3 ; j++)
                _g3[Perlin::_B + i][j] = _g3[i][j];
        }
    }

    Perlin::~Perlin()
    {
    }

    void Perlin::_Normalize2(double v[2])
    {
        double s;

        s = sqrt(v[0] * v[0] + v[1] * v[1]);
        v[0] = v[0] / s;
        v[1] = v[1] / s;
    }

    void Perlin::_Normalize3(double v[3])
    {
        double s;

        s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        v[0] = v[0] / s;
        v[1] = v[1] / s;
        v[2] = v[2] / s;
    }

    double Perlin::_Noise1(double arg) const
    {
        int bx0, bx1;
        double rx0, rx1, sx, t, u, v, vec[1];

        vec[0] = arg;

        Perlin::_Setup(0, bx0, bx1, rx0, rx1, vec, t);

        sx = Perlin::_SCurve(rx0);
        u = rx0 * _g1[_p[bx0]];
        v = rx1 * _g1[_p[bx1]];

        return Perlin::_Lerp(sx, u, v);
    }

    double Perlin::_Noise2(double vec[2]) const
    {
        int bx0, bx1, by0, by1, b00, b10, b01, b11;
        double rx0, rx1, ry0, ry1, sx, sy, a, b, t, u, v;
        double const* q;
        int i, j;

        Perlin::_Setup(0, bx0, bx1, rx0, rx1, vec, t);
        Perlin::_Setup(1, by0, by1, ry0, ry1, vec, t);

        i = _p[bx0];
        j = _p[bx1];

        b00 = _p[i + by0];
        b10 = _p[j + by0];
        b01 = _p[i + by1];
        b11 = _p[j + by1];

        sx = Perlin::_SCurve(rx0);
        sy = Perlin::_SCurve(ry0);

        q = _g2[b00];
        u = Perlin::_At2(rx0, ry0, q);
        q = _g2[b10];
        v = Perlin::_At2(rx1, ry0, q);
        a = Perlin::_Lerp(sx, u, v);

        q = _g2[b01];
        u = Perlin::_At2(rx0, ry1, q);
        q = _g2[b11];
        v = Perlin::_At2(rx1, ry1, q);
        b = Perlin::_Lerp(sx, u, v);

        return Perlin::_Lerp(sy, a, b);
    }

    double Perlin::_Noise3(double vec[3]) const
    {
        int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
        double rx0, rx1, ry0, ry1, rz0, rz1, sy, sz, a, b, c, d, t, u, v;
        double const* q;
        int i, j;

        Perlin::_Setup(0, bx0, bx1, rx0, rx1, vec, t);
        Perlin::_Setup(1, by0, by1, ry0, ry1, vec, t);
        Perlin::_Setup(2, bz0, bz1, rz0, rz1, vec, t);

        i = _p[bx0];
        j = _p[bx1];

        b00 = _p[i + by0];
        b10 = _p[j + by0];
        b01 = _p[i + by1];
        b11 = _p[j + by1];

        t  = Perlin::_SCurve(rx0);
        sy = Perlin::_SCurve(ry0);
        sz = Perlin::_SCurve(rz0);

        q = _g3[b00 + bz0] ; u = Perlin::_At3(rx0, ry0, rz0, q);
        q = _g3[b10 + bz0] ; v = Perlin::_At3(rx1, ry0, rz0, q);
        a = Perlin::_Lerp(t, u, v);

        q = _g3[b01 + bz0] ; u = Perlin::_At3(rx0, ry1, rz0, q);
        q = _g3[b11 + bz0] ; v = Perlin::_At3(rx1, ry1, rz0, q);
        b = Perlin::_Lerp(t, u, v);

        c = Perlin::_Lerp(sy, a, b);

        q = _g3[b00 + bz1] ; u = Perlin::_At3(rx0, ry0, rz1, q);
        q = _g3[b10 + bz1] ; v = Perlin::_At3(rx1, ry0, rz1, q);
        a = Perlin::_Lerp(t, u, v);

        q = _g3[b01 + bz1] ; u = Perlin::_At3(rx0, ry1, rz1, q);
        q = _g3[b11 + bz1] ; v = Perlin::_At3(rx1, ry1, rz1, q);
        b = Perlin::_Lerp(t, u, v);

        d = Perlin::_Lerp(sy, a, b);

        return Perlin::_Lerp(sz, c, d);
    }



    // In what follows "alpha" is the weight when the sum is formed.
    // Typically it is 2, As this approaches 1 the function is noisier.
    // "beta" is the harmonic scaling/spacing, typically 2.

    double Perlin::Noise1D(double x, double alpha, double beta, int n) const
    {
        int i;
        double val;
        double sum = 0;
        double p;
        double scale = 1;

        p = x;
        for (i = 0 ; i < n ; i++)
        {
            val = this->_Noise1(p);
            sum += val / scale;
            scale *= alpha;
            p *= beta;
        }
        return sum;
    }

    double Perlin::Noise2D(double x, double y, double alpha, double beta, int n) const
    {
        int i;
        double val;
        double sum = 0;
        double p[2];
        double scale = 1;

        p[0] = x;
        p[1] = y;
        for (i = 0 ; i < n ; i++)
        {
            val = this->_Noise2(p);
            sum += val / scale;
            scale *= alpha;
            p[0] *= beta;
            p[1] *= beta;
        }
        return sum;
    }

    double Perlin::Noise3D(double x, double y, double z, double alpha, double beta, int n) const
    {
        int i;
        double val;
        double sum = 0;
        double p[3];
        double scale = 1;

        p[0] = x;
        p[1] = y;
        p[2] = z;
        for (i = 0 ; i < n ; i++)
        {
            val = this->_Noise3(p);
            sum += val / scale;
            scale *= alpha;
            p[0] *= beta;
            p[1] *= beta;
            p[2] *= beta;
        }
        return sum;
    }

}}}}
