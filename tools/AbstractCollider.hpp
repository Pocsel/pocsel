#ifndef __TOOLS_ABSTRACTCOLLIDER_HPP__
#define __TOOLS_ABSTRACTCOLLIDER_HPP__

#include "tools/Vector3.hpp"

namespace Tools {

    class AlignedBox;
    class Frustum;
    class Plane;

    class AbstractCollider
    {
    protected:
        AbstractCollider() {}

    public:
        enum IntersectionType
        {
            Outside,
            Intersecting,
            Inside
        };

        bool Intersects(AbstractCollider const& object) const
        {
            return this->Contains(object) != Outside;
        }

        virtual IntersectionType Contains(AbstractCollider const&) const
        {
            return Outside;
        }

        virtual IntersectionType Contains(Vector3d const& object) const = 0;
        virtual IntersectionType Contains(AlignedBox const& object) const = 0;
        virtual IntersectionType Contains(Frustum const& object) const = 0;
    };

}

#endif
