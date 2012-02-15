#include "tools/AlignedBox.hpp"
#include "tools/AlignedCube.hpp"
#include "tools/Frustum.hpp"
#include "tools/Plane.hpp"

namespace Tools {

    Vector3d AlignedCube::SupportMapping(Vector3d const& v) const
    {
        return Vector3d(
            (v.x >= 0) ? this->_position.x + this->_size : this->_position.x,
            (v.y >= 0) ? this->_position.y + this->_size : this->_position.y,
            (v.z >= 0) ? this->_position.z + this->_size : this->_position.z);
    }

    Plane::IntersectionType AlignedCube::Intersects(Plane const& plane) const
    {
        Vector3d vector;
        vector.x = (plane.normal.x >= 0) ? this->_position.x : this->_position.x + this->_size;
        vector.y = (plane.normal.y >= 0) ? this->_position.y : this->_position.y + this->_size;
        vector.z = (plane.normal.z >= 0) ? this->_position.z : this->_position.z + this->_size;
        if ((Vector3d::Dot(plane.normal, vector) + plane.d) > 0)
            return Plane::Front;

        vector.x = (plane.normal.x >= 0) ? this->_position.x + this->_size : this->_position.x;
        vector.y = (plane.normal.y >= 0) ? this->_position.y + this->_size : this->_position.y;
        vector.z = (plane.normal.z >= 0) ? this->_position.z + this->_size : this->_position.z;
        if ((Vector3d::Dot(plane.normal, vector) + plane.d) < 0)
            return Plane::Back;

        return Plane::Intersecting;
    }

    AbstractCollider::IntersectionType AlignedCube::Contains(Vector3d const& point) const
    {
        if (this->_position.x <= point.x && point.x <= this->_position.x + this->_size &&
            this->_position.y <= point.y && point.y <= this->_position.y + this->_size &&
            this->_position.z <= point.z && point.z <= this->_position.z + this->_size)
            return Inside;
        return Outside;
    }

    AbstractCollider::IntersectionType AlignedCube::Contains(AlignedBox const& box) const
    {
        if (this->_position.x + this->_size < box.GetMin().x || this->_position.x > box.GetMax().x ||
            this->_position.y + this->_size < box.GetMin().y || this->_position.y > box.GetMax().y ||
            this->_position.z + this->_size < box.GetMin().z || this->_position.z > box.GetMax().z)
            return Outside;
        if (this->_position.x <= box.GetMin().x && box.GetMax().x <= this->_position.x + this->_size &&
            this->_position.y <= box.GetMin().y && box.GetMax().y <= this->_position.y + this->_size &&
            this->_position.z <= box.GetMin().z && box.GetMax().z <= this->_position.z + this->_size)
            return Inside;
        return Intersecting;
    }

    AbstractCollider::IntersectionType AlignedCube::Contains(AlignedCube const& box) const
    {
        if (this->_position.x + this->_size < box._position.x || this->_position.x > box._position.x + box._size ||
            this->_position.y + this->_size < box._position.y || this->_position.y > box._position.y + box._size ||
            this->_position.z + this->_size < box._position.z || this->_position.z > box._position.z + box._size)
            return Outside;
        if (this->_position.x <= box._position.x && box._position.x + box._size <= this->_position.x + this->_size &&
            this->_position.y <= box._position.y && box._position.y + box._size <= this->_position.y + this->_size &&
            this->_position.z <= box._position.z && box._position.z + box._size <= this->_position.z + this->_size)
            return Inside;
        return Intersecting;
    }

    AbstractCollider::IntersectionType AlignedCube::Contains(Frustum const& frustum) const
    {
        if (!frustum.Intersects(*this))
            return Outside;
        for (int i = 0; i < 8; ++i)
            if (this->Contains(frustum.GetCorners()[i]) == Outside)
                return Intersecting;
        return Inside;
    }

}
