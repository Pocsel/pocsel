#ifndef __GLM_STD_UTILS_HPP__
#define __GLM_STD_UTILS_HPP__

#include "glm/core/type_vec3.hpp"
#include "glm/core/type_vec4.hpp"


namespace std
{

    template<typename T>
    bool operator < (glm::detail::tvec4<T> const& left, glm::detail::tvec4<T> const& right)
    {
        return
            (left.x < right.x ||
             (left.x == right.x &&
              (left.y < right.y ||
               (left.y == right.y &&
                (left.z < right.z ||
                 (left.z == right.z &&
                  left.w < right.w
                 ))))));
//        return glm::length(left) < glm::length(right);
    }

    template<typename T>
    bool operator < (glm::detail::tvec3<T> const& left, glm::detail::tvec3<T> const& right)
    {
        return
            (left.x < right.x ||
             (left.x == right.x &&
              (left.y < right.y ||
               (left.y == right.y &&
                left.z < right.z
               ))));
//        return glm::length(left) < glm::length(right);
    }

}


#endif
