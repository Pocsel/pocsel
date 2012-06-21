#ifndef __COMMON_PHYSICS_NODE_HPP__
#define __COMMON_PHYSICS_NODE_HPP__

#include "common/physics/Vector.hpp"
#include "common/Position.hpp"

namespace Common { namespace Physics {

    struct Node
    {
        Common::Position position;
        Common::Position velocity;
        Common::Position acceleration;

        glm::quat orientation;
        glm::vec3 angularVelocity;
        glm::vec3 angularAcceleration;

        glm::vec3 scale;
        glm::vec3 scaleVelocity;
        glm::vec3 scaleAcceleration;

       // Vector<double> position;
       // Vector<float> yawPitchRoll;
       // Vector<float> scale;
    };

}}

#endif
