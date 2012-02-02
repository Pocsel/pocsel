#ifndef __TOOLS_RAY_HPP__
#define __TOOLS_RAY_HPP__

#include "Vector3.hpp"

namespace Tools {

    class Ray
    {
    public:
        Vector3d position;
        Vector3d direction;

    public:
        Ray() {}
    };

}

#endif
