#ifndef __COMMON_PHYSICS_VECTOR_HPP__
#define __COMMON_PHYSICS_VECTOR_HPP__

namespace Common { namespace Physics {

    template<typename T = float>
    struct Vector
    {
        glm::detail::tvec3<T> r;
        glm::detail::tvec3<T> v;
        glm::detail::tvec3<T> a;
    };

}}

#endif
