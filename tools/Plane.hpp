#ifndef __TOOLS_PLANE_HPP__
#define __TOOLS_PLANE_HPP__

#include "tools/AbstractCollider.hpp"
#include "tools/Vector3.hpp"

namespace Tools {

    class Ray;

    class Plane
    {
    public:
        enum IntersectionType
        {
            Intersecting,
            Front,
            Back
        };

        Vector3d normal;
        double d;

        Plane() {}
        Plane(double a, double b, double c, double d);
        Plane(Vector3d const& normal, double d);
        Plane(Vector3d const& pt1, Vector3d const& pt2, Vector3d const& pt3);

        void Normalize();

        IntersectionType Intersects(AlignedBox const& object) const;
        IntersectionType Intersects(Frustum const& object) const;

        Ray GetIntersection(Plane const& plane) const;
        Vector3d GetIntersection(Ray const& ray) const;
    };

}

#endif
