#ifndef __TOOLS_PHYSICSOBJECT_HPP__
#define __TOOLS_PHYSICSOBJECT_HPP__

#include "tools/ByteArray.hpp"
#include "tools/Vector2.hpp"
#include "tools/Vector3.hpp"

namespace Tools {

    struct PhysicsObject
    {
        glm::detail::dvec3 position;
        glm::detail::fvec3 speed;
        glm::detail::fvec3 acceleration;

        glm::detail::fvec3 yawPitchRoll;
        glm::detail::fvec3 yawPitchRollSpeed;

        glm::detail::fvec3 size;

        void SetPosition(
    };

}

#endif


