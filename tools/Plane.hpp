#ifndef __TOOLS_PLANE_HPP__
#define __TOOLS_PLANE_HPP__

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

        glm::dvec3 normal;
        double d;

        Plane() {}
        Plane(double a, double b, double c, double d);
        Plane(glm::dvec3 const& normal, double d);
        Plane(glm::dvec3 const& pt1, glm::dvec3 const& pt2, glm::dvec3 const& pt3);

        void Normalize();

        IntersectionType Intersects(AlignedBox const& object) const;
        IntersectionType Intersects(Frustum const& object) const;

        Ray GetIntersection(Plane const& plane) const;
        glm::dvec3 GetIntersection(Ray const& ray) const;
    };

}

#endif
