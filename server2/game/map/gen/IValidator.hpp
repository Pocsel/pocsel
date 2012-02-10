#ifndef __SERVER_GAME_MAP_GEN_IVALIDATOR_HPP__
#define __SERVER_GAME_MAP_GEN_IVALIDATOR_HPP__

namespace Server { namespace Game { namespace Map { namespace Gen {

    class IValidator
    {
    public:
        virtual bool Check(double x, double y, double z, double eq) const = 0;
    };

}}}}

#endif
