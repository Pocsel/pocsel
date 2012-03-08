
#include "mapgen/RandMersenneTwister.hpp"
#include "mapgen/Perlin.hpp"
#include "perlin.h"

static inline void cppbite()
{
    Server::MapGen::RandMersenneTwister r(421);
    Server::MapGen::Perlin p(r);
    auto startTime = boost::get_system_time().time_of_day().total_milliseconds();
    for (int x = 0 ; x < 128 ; ++x)
    {
        double xx = x;
        for (int y = 0 ; y < 128 ; ++y)
        {
            double yy = y;
            for (int z = 0 ; z < 128 ; ++z)
            {
                double zz = z;
                p.Noise3D(xx, yy, zz, 2.0, 2.0, 4);
            }
        }
    }
    std::cout << "cpp time= " << (boost::get_system_time().time_of_day().total_milliseconds() - startTime) << "ms\n";
}
static inline void cbite()
{
    auto startTime = boost::get_system_time().time_of_day().total_milliseconds();
    for (int x = 0 ; x < 128 ; ++x)
    {
        double xx = x;
        for (int y = 0 ; y < 128 ; ++y)
        {
            double yy = y;
            for (int z = 0 ; z < 128 ; ++z)
            {
                double zz = z;
                PerlinNoise3D(xx, yy, zz, 2.0, 2.0, 4);
            }
        }
    }
    std::cout << "c time= " << (boost::get_system_time().time_of_day().total_milliseconds() - startTime) << "ms\n";
}

int main()
{
    cppbite();
    cppbite();
    cppbite();
    cppbite();
    cppbite();
    cbite();
    cbite();
    cbite();
    cbite();
    cbite();
}

