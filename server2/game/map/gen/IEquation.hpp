#ifndef __SERVER_GAME_MAP_GEN_IEQUATION_HPP__
#define __SERVER_GAME_MAP_GEN_IEQUATION_HPP__

namespace Server { namespace Game { namespace Map { namespace Gen {

    class IEquation
    {
    public:
        virtual double Calc(double x, double y, double z) const = 0;
        virtual bool Is2D() const = 0;
    };

}}}}

#endif