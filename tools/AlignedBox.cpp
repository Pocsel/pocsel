
#include "AlignedBox.hpp"
#include "Frustum.hpp"
#include "Plane.hpp"

namespace Tools {

    Vector3d AlignedBox::SupportMapping(Vector3d const& v) const
    {
        return Vector3d(
            (v.x >= 0) ? this->_max.x : this->_min.x,
            (v.y >= 0) ? this->_max.y : this->_min.y,
            (v.z >= 0) ? this->_max.z : this->_min.z);
    }

    bool AlignedBox::Intersects(AlignedBox const& box) const
    {
        if (this->_max.x < box._min.x || this->_min.x > box._max.x ||
            this->_max.y < box._min.y || this->_min.y > box._max.y ||
            this->_max.z < box._min.z || this->_min.z > box._max.z)
            return false;
        return true;
    }

    bool AlignedBox::Intersects(Frustum const& object) const
    {
        return object.Intersects(*this);
    }

    Plane::IntersectionType AlignedBox::Intersects(Plane const& plane) const
    {
        Vector3d vector;
        vector.x = (plane.normal.x >= 0) ? this->_min.x : this->_max.x;
        vector.y = (plane.normal.y >= 0) ? this->_min.y : this->_max.y;
        vector.z = (plane.normal.z >= 0) ? this->_min.z : this->_max.z;
        if ((Vector3d::Dot(plane.normal, vector) + plane.d) > 0)
            return Plane::Front;

        vector.x = (plane.normal.x >= 0) ? this->_max.x : this->_min.x;
        vector.y = (plane.normal.y >= 0) ? this->_max.y : this->_min.y;
        vector.z = (plane.normal.z >= 0) ? this->_max.z : this->_min.z;
        if ((Vector3d::Dot(plane.normal, vector) + plane.d) < 0)
            return Plane::Back;

        return Plane::Intersecting;
    }

    AbstractCollider::IntersectionType AlignedBox::Contains(Vector3d const& point) const
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
