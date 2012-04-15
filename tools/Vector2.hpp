#ifndef __TOOLS_VECTOR2_HPP__
#define __TOOLS_VECTOR2_HPP__

#include "tools/sqrt.hpp"

namespace Tools {

    template<typename T>
    struct Stringify<glm::detail::tvec2<T>>
    {
        static inline std::string MakeString(glm::detail::tvec2<T> const& var)
        {
            return "(" + ToString(var.x) + ", " + ToString(var.y) + ")";
        }
    };

}

#endif
