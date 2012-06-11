#ifndef __TOOLS_PHYSICS_NODE_HPP__
#define __TOOLS_PHYSICS_NODE_HPP__

#include "tools/physics/Vector.hpp"

namespace Tools { namespace Physics {

    struct Node
    {
        Vector<double, float, float> position;
        Vector<> yawPitchRoll;
        Vector<> scale;
    };

}}

#endif
