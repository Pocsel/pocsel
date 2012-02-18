#ifndef __SERVER_GAME_MAP_GEN_IEQUATION_HPP__
#define __SERVER_GAME_MAP_GEN_IEQUATION_HPP__

namespace Server { namespace Game { namespace Map { namespace Gen {

    class IEquation
    {
    public:
//        virtual double Calc(double x, double y, double z) const = 0;
//        virtual bool Is2D() const = 0;
//        virtual bool ByChunkCalculation() const = 0;
        virtual void Calc(double x, double y, double z, Uint32 cx, Uint32 cy, Uint32 cz, double* res, unsigned int offset) const = 0;
    };

}}}}

#endif
