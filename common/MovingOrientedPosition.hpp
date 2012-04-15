#ifndef __COMMON_MOVINGORIENTEDPOSITION_HPP__
#define __COMMON_MOVINGORIENTEDPOSITION_HPP__

#include "common/OrientedPosition.hpp"

namespace Common {

    struct MovingOrientedPosition
    {
    public:
        Common::OrientedPosition position;
        glm::fvec3 movement;

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

        MovingOrientedPosition(Common::OrientedPosition const& pos, glm::fvec3 const& movement) :
            position(pos),
            movement(movement)
        {
        }

        void Move(float time)
        {
            position.position += glm::dvec3(movement) * (double)time;
        }

    };

}

#endif

