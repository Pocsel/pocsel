#ifndef __COMMON_PHYSICS_NODE_HPP__
#define __COMMON_PHYSICS_NODE_HPP__

#include "common/physics/Vector.hpp"
#include "common/Position.hpp"

namespace Common { namespace Physics {

    struct Node
    {
        Vector<PositionCoordType, float, float> position;
        Vector<> yawPitchRoll;
        Vector<> scale;
    };

}}

#endif
