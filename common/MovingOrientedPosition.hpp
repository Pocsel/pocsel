#ifndef __COMMON_MOVINGORIENTEDPOSITION_HPP__
#define __COMMON_MOVINGORIENTEDPOSITION_HPP__

#include "common/OrientedPosition.hpp"

namespace Common {

    struct MovingOrientedPosition
    {
    public:
        Common::OrientedPosition position;
        Tools::Vector3f movement;

    public:
        MovingOrientedPosition() :
            position(),
            movement()
        {
        }

        MovingOrientedPosition(Common::Position const& pos) :
            position(pos),
            movement()
        {
        }

        MovingOrientedPosition(Common::Position const& pos, float theta, float phi) :
            position(pos, theta, phi),
            movement()
        {
        }

        MovingOrientedPosition(Common::OrientedPosition const& pos) :
            position(pos),
            movement()
        {
        }

        MovingOrientedPosition(Common::OrientedPosition const& pos, Tools::Vector3f const& movement) :
            position(pos),
            movement(movement)
        {
        }

        void Move(float time)
        {
            position.position.AddOffset(movement * time);
        }

    };

}

#endif

