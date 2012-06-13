#ifndef __COMMON_PHYSICS_NODE_HPP__
#define __COMMON_PHYSICS_NODE_HPP__

#include "common/physics/Vector.hpp"
#include "common/Position.hpp"

namespace Common { namespace Physics {

    struct Node
    {
        Vector<double> position;
        Vector<float> yawPitchRoll;
        Vector<float> scale;
    };

}}

#endif
