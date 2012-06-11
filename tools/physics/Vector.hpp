#ifndef __TOOLS_PHYSICS_VECTOR_HPP__
#define __TOOLS_PHYSICS_VECTOR_HPP__

namespace Tools { namespace Physics {

    template<typename RT = float, typename VT = float, typename AT = float>
    struct Vector
    {
        glm::detail::tvec3<RT> r;
        glm::detail::tvec3<VT> v;
        glm::detail::tvec3<AT> a;
    };

}}

#endif
