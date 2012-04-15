#include "tools/AlignedBox.hpp"
#include "tools/AlignedCube.hpp"
#include "tools/Frustum.hpp"
#include "tools/Plane.hpp"

namespace Tools {

    glm::dvec3 AlignedBox::SupportMapping(glm::dvec3 const& v) const
    {
        return glm::dvec3(
            (v.x >= 0) ? this->_max.x : this->_min.x,
            (v.y >= 0) ? this->_max.y : this->_min.y,
            (v.z >= 0) ? this->_max.z : this->_min.z);
    }

    Plane::IntersectionType AlignedBox::Intersects(Plane const& plane) const
    {
        glm::dvec3 vector;
        vector.x = (plane.normal.x >= 0) ? this->_min.x : this->_max.x;
        vector.y = (plane.normal.y >= 0) ? this->_min.y : this->_max.y;
        vector.z = (plane.normal.z >= 0) ? this->_min.z : this->_max.z;
        if ((glm::dot(plane.normal, vector) + plane.d) > 0)
            return Plane::Front;

        vector.x = (plane.normal.x >= 0) ? this->_max.x : this->_min.x;
        vector.y = (plane.normal.y >= 0) ? this->_max.y : this->_min.y;
        vector.z = (plane.normal.z >= 0) ? this->_max.z : this->_min.z;
        if ((glm::dot(plane.normal, vector) + plane.d) < 0)
            return Plane::Back;

        return Plane::Intersecting;
    }

    AbstractCollider::IntersectionType AlignedBox::Contains(glm::dvec3 const& point) const
    {
        if (this->_min.x <= point.x && point.x <= this->_max.x &&
            this->_min.y <= point.y && point.y <= this->_max.y &&
            this->_min.z <= point.z && point.z <= this->_max.z)
            return Inside;
        return Outside;
    }

    AbstractCollider::IntersectionType AlignedBox::Contains(AlignedBox const& box) const
    {
        if (this->_max.x < box._min.x || this->_min.x > box._max.x ||
            this->_max.y < box._min.y || this->_min.y > box._max.y ||
            this->_max.z < box._min.z || this->_min.z > box._max.z)
            return Outside;
        if (this->_min.x <= box._min.x && box._max.x <= this->_max.x &&
            this->_min.y <= box._min.y && box._max.y <= this->_max.y &&
            this->_min.z <= box._min.z && box._max.z <= this->_max.z)
            return Inside;
        return Intersecting;
    }

    AbstractCollider::IntersectionType AlignedBox::Contains(AlignedCube const& box) const
    {
        if (this->_max.x < box.GetPosition().x || this->_min.x > box.GetPosition().x + box.GetSize() ||
            this->_max.y < box.GetPosition().y || this->_min.y > box.GetPosition().y + box.GetSize() ||
            this->_max.z < box.GetPosition().z || this->_min.z > box.GetPosition().z + box.GetSize())
            return Outside;
        if (this->_min.x <= box.GetPosition().x && box.GetPosition().x + box.GetSize() <= this->_max.x &&
            this->_min.y <= box.GetPosition().y && box.GetPosition().y + box.GetSize() <= this->_max.y &&
            this->_min.z <= box.GetPosition().z && box.GetPosition().z + box.GetSize() <= this->_max.z)
            return Inside;
        return Intersecting;
    }

    AbstractCollider::IntersectionType AlignedBox::Contains(Frustum const& frustum) const
    {
        if (!frustum.Intersects(*this))
            return Outside;
        for (int i = 0; i < 8; ++i)
            if (this->Contains(frustum.GetCorners()[i]) == Outside)
                return Intersecting;
        return Inside;
    }

}
