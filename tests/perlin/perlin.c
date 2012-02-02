/* Coherent noise function over 1, 2 or 3 dimensions */
/* (copyright Ken Perlin) */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "perlin.h"

//
// Mersenne twister RANDOM
//

static int my_rand_MT[624];
static int my_rand_index = 0;

void my_srand(int seed)
{
    int i;
    my_rand_MT[0] = seed;

    for (i = 1 ; i < 624 ; ++i)
        my_rand_MT[i] =
            1812433253 * (my_rand_MT[i - 1] ^ (my_rand_MT[i - 1] >> 30)) + i;
}

static void my_rand_generateNumbers()
{
    int y;

    int i;
    for (i = 0; i < 624 ; ++i)
    {
        y = (my_rand_MT[i] & 0x80000000) +
            ((my_rand_MT[(i + 1) % 624]) & 0x7FFFFFFF);

        my_rand_MT[i] = my_rand_MT[(i + 397) % 624] ^ ((y) >> 1);
        if (y % 2)
            my_rand_MT[i] = my_rand_MT[i] ^ (2567483615);
    }
}

int my_rand()
{
    int y;
    if (my_rand_index == 624)
    {
        my_rand_generateNumbers();
        my_rand_index = 0;
    }

    y = my_rand_MT[my_rand_index];
    y = y ^ ((y) >> 11);
    y = y ^ ((y << 7) & (2636928640));
    y = y ^ ((y << 15) & (4022730752));
    y = y ^ ((y >> 18));

    ++my_rand_index;
    return y;
}


//
//
//

static int p[B + B + 2];
static double g3[B + B + 2][3];
static double g2[B + B + 2][2];
static double g1[B + B + 2];
static int start = 1;

double noise1(double arg)
{
    int bx0, bx1;
    double rx0, rx1, sx, t, u, v, vec[1];

    vec[0] = arg;
    if (start)
    {
        start = 0;
        init();
    }

    setup(0, bx0, bx1, rx0, rx1);

    sx = s_curve(rx0);
    u = rx0 * g1[p[bx0]];
    v = rx1 * g1[p[bx1]];

    return lerp(sx, u, v);
}

double noise2(double vec[2])
{
    int bx0, bx1, by0, by1, b00, b10, b01, b11;
    double rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
    int i, j;

    if (start)
    {
        start = 0;
        init();
    }

    setup(0, bx0, bx1, rx0, rx1);
    setup(1, by0, by1, ry0, ry1);

    i = p[bx0];
    j = p[bx1];

    b00 = p[i + by0];
    b10 = p[j + by0];
    b01 = p[i + by1];
    b11 = p[j + by1];

    sx = s_curve(rx0);
    sy = s_curve(ry0);

    q = g2[b00] ; u = at2(rx0, ry0);
    q = g2[b10] ; v = at2(rx1, ry0);
    a = lerp(sx, u, v);

    q = g2[b01] ; u = at2(rx0, ry1);
    q = g2[b11] ; v = at2(rx1, ry1);
    b = lerp(sx, u, v);

    return lerp(sy, a, b);
}

double noise3(double vec[3])
{
    int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
    double rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
    int i, j;

    if (start)
    {
        start = 0;
        init();
    }

    setup(0, bx0, bx1, rx0, rx1);
    setup(1, by0, by1, ry0, ry1);
    setup(2, bz0, bz1, rz0, rz1);

    i = p[bx0];
    j = p[bx1];

    b00 = p[i + by0];
    b10 = p[j + by0];
    b01 = p[i + by1];
    b11 = p[j + by1];

    t  = s_curve(rx0);
    sy = s_curve(ry0);
    sz = s_curve(rz0);

    q = g3[b00 + bz0] ; u = at3(rx0, ry0, rz0);
    q = g3[b10 + bz0] ; v = at3(rx1, ry0, rz0);
    a = lerp(t, u, v);

    q = g3[b01 + bz0] ; u = at3(rx0, ry1, rz0);
    q = g3[b11 + bz0] ; v = at3(rx1, ry1, rz0);
    b = lerp(t, u, v);

    c = lerp(sy, a, b);

    q = g3[b00 + bz1] ; u = at3(rx0, ry0, rz1);
    q = g3[b10 + bz1] ; v = at3(rx1, ry0, rz1);
    a = lerp(t, u, v);

    q = g3[b01 + bz1] ; u = at3(rx0, ry1, rz1);
    q = g3[b11 + bz1] ; v = at3(rx1, ry1, rz1);
    b = lerp(t, u, v);

    d = lerp(sy, a, b);

    return lerp(sz, c, d);
}

void normalize2(double v[2])
{
    double s;

    s = sqrt(v[0] * v[0] + v[1] * v[1]);
    v[0] = v[0] / s;
    v[1] = v[1] / s;
}

void normalize3(double v[3])
{
    double s;

    s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    v[0] = v[0] / s;
    v[1] = v[1] / s;
    v[2] = v[2] / s;
}

void init(void)
{
    int i, j, k;

    for (i = 0 ; i < B ; i++)
    {
        p[i] = i;
        g1[i] = (double)((my_rand() % (B + B)) - B) / B;

        for (j = 0 ; j < 2 ; j++)
            g2[i][j] = (double)((my_rand() % (B + B)) - B) / B;
        normalize2(g2[i]);

        for (j = 0 ; j < 3 ; j++)
            g3[i][j] = (double)((my_rand() % (B + B)) - B) / B;
        normalize3(g3[i]);
    }

    while (--i)
    {
        k = p[i];
        p[i] = p[j = my_rand() % B];
        p[j] = k;
    }

    for (i = 0 ; i < B + 2 ; i++)
    {
        p[B + i] = p[i];
        g1[B + i] = g1[i];
        for (j = 0 ; j < 2 ; j++)
            g2[B + i][j] = g2[i][j];
        for (j = 0 ; j < 3 ; j++)
            g3[B + i][j] = g3[i][j];
    }
}

/* --- My harmonic summing functions - PDB --------------------------*/

/*
In what follows "alpha" is the weight when the sum is formed.
Typically it is 2, As this approaches 1 the function is noisier.
"beta" is the harmonic scaling/spacing, typically 2.
*/

double PerlinNoise1D(double x,double alpha,double beta,int n)
{
    int i;
    double val;
    double sum = 0;
    double p;
    double scale = 1;

    p = x;
    for (i = 0 ; i < n ; i++)
    {
        val = noise1(p);
        sum += val / scale;
        scale *= alpha;
        p *= beta;
    }
    return sum;
}

double PerlinNoise2D(double x,double y,double alpha,double beta,int n)
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
        val = noise2(p);
        sum += val / scale;
        scale *= alpha;
        p[0] *= beta;
        p[1] *= beta;
    }
    return sum;
}

double PerlinNoise3D(double x,double y,double z,double alpha,double beta,int n)
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
        val = noise3(p);
        sum += val / scale;
        scale *= alpha;
        p[0] *= beta;
        p[1] *= beta;
        p[2] *= beta;
    }
    return sum;
}
