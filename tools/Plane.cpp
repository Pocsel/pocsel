#include "tools/AlignedBox.hpp"
#include "tools/Frustum.hpp"
#include "tools/Plane.hpp"
#include "tools/Ray.hpp"
#include "tools/Vector3.hpp"

namespace Tools {

    Plane::Plane(double a, double b, double c, double d) :
        normal(a, b, c),
        d(d)
    {
    }

    Plane::Plane(glm::dvec3 const& normal, double d) :
        normal(normal),
        d(d)
    {
    }

    Plane::Plane(glm::dvec3 const& pt1, glm::dvec3 const& pt2, glm::dvec3 const& pt3)
    {
        double dx = pt2.x - pt1.x;
        double dy = pt2.y - pt1.y;
        double dz = pt2.z - pt1.z;
        double dx2 = pt3.x - pt1.x;
        double dy2 = pt3.y - pt1.y;
        double dz2 = pt3.z - pt1.z;
        this->normal.x = dy * dz2 - dz * dy2;
        this->normal.y = dz * dx2 - dx * dz2;
        this->normal.z = dx * dy2 - dy * dx2;
        this->d = -(this->normal.x * pt1.x + this->normal.y * pt1.y + this->normal.z * pt1.z);
        this->normal = glm::normalize(this->normal);
    }

    void Plane::Normalize()
    {
        double sq = this->normal.x*this->normal.x + this->normal.y*this->normal.y + this->normal.z*this->normal.z;
        double mag = 1 / std::sqrt(sq);
        this->normal.x = this->normal.x * mag;
        this->normal.y = this->normal.y * mag;
        this->normal.z = this->normal.z * mag;
        this->d *= mag;
    }

    Plane::IntersectionType Plane::Intersects(AlignedBox const& box) const
    {
        return box.Intersects(*this);
    }

    Plane::IntersectionType Plane::Intersects(Frustum const& frustum) const
    {
        int result = 0;
        for (int i = 0; i < 8; i++)
        {
            result |= ((glm::dot(frustum.GetCorners()[i], this->normal) + this->d) > 0) ? 1 : 2;
            if (result == 3)
                return Intersecting;
        }
        return (result != 1 ? Back : Front);
    }

    Ray Plane::GetIntersection(Plane const& plane) const
    {
        Ray r;
        r.direction = glm::cross(this->normal, plane.normal);
        r.position = glm::cross(-this->d * plane.normal + plane.d * this->normal, r.direction) / glm::lengthSquared(r.direction);
        return r;
    }

    glm::dvec3 Plane::GetIntersection(Ray const& ray) const
    {
        double scaleFactor = (-this->d - glm::dot(this->normal, ray.position)) / glm::dot(this->normal, ray.direction);
        return ray.position + ray.direction * scaleFactor;
    }

}
