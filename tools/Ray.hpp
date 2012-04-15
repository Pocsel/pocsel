#ifndef __TOOLS_RAY_HPP__
#define __TOOLS_RAY_HPP__

#include "Vector3.hpp"

namespace Tools {

    class Ray
    {
    public:
        glm::dvec3 position;
        glm::dvec3 direction;

    public:
        Ray() {}
    };

}

#endif
