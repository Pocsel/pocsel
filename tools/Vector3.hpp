#ifndef __TOOLS_VECTOR3_HPP__
#define __TOOLS_VECTOR3_HPP__

#include "tools/sqrt.hpp"
#include "tools/Vector2.hpp"

namespace Tools {

    template<typename T>
    struct Stringify<glm::detail::tvec3<T>>
    {
        static inline std::string MakeString(glm::detail::tvec3<T> const& var)
        {
            return "(" + ToString(var.x) + ", " + ToString(var.y) + ", " + ToString(var.z) + ")";
        }
    };

}

#endif
