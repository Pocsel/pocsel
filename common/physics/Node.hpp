#ifndef __COMMON_PHYSICS_NODE_HPP__
#define __COMMON_PHYSICS_NODE_HPP__

#include "common/Position.hpp"

namespace Common { namespace Physics {

    struct Node
    {
        Common::Position position;
        Common::Position velocity;

        glm::dquat orientation;
        glm::dvec3 angularVelocity;

        //Acceleration accel;
        bool accelerationIsLocal;
        Common::Position acceleration;
        double maxSpeed;

        //trucs inter-noeuds
        glm::dvec3 interPosition;
        glm::dvec3 interAngle;
        glm::dvec3 interPositionTarget;
        double interPositionTargetSpeed;
        glm::dvec3 interAngleTarget;
        double interAngleTargetSpeed;
        glm::dvec3 interAngleLimits;

        Node() : accelerationIsLocal(false) {}
    };

}}

#endif
