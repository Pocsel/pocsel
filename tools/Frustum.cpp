#include "tools/AlignedBox.hpp"
#include "tools/AlignedCube.hpp"
#include "tools/Frustum.hpp"
#include "tools/Ray.hpp"

namespace Tools {

    Frustum::Frustum(glm::detail::tmat4x4<double> const& view)
    {
        this->_matrix = glm::scale(1.0, 1.0, 0.5) * glm::translate(0.0, 0.0, 1.0) * view;
        this->_planes[2].normal.x = -this->_matrix[0][3] - this->_matrix[0][0];
        this->_planes[2].normal.y = -this->_matrix[1][3] - this->_matrix[1][0];
        this->_planes[2].normal.z = -this->_matrix[2][3] - this->_matrix[2][0];
        this->_planes[2].d = -this->_matrix[3][3] - this->_matrix[3][0];
        this->_planes[3].normal.x = -this->_matrix[0][3] + this->_matrix[0][0];
        this->_planes[3].normal.y = -this->_matrix[1][3] + this->_matrix[1][0];
        this->_planes[3].normal.z = -this->_matrix[2][3] + this->_matrix[2][0];
        this->_planes[3].d = -this->_matrix[3][3] + this->_matrix[3][0];
        this->_planes[4].normal.x = -this->_matrix[0][3] + this->_matrix[0][1];
        this->_planes[4].normal.y = -this->_matrix[1][3] + this->_matrix[1][1];
        this->_planes[4].normal.z = -this->_matrix[2][3] + this->_matrix[2][1];
        this->_planes[4].d = -this->_matrix[3][3] + this->_matrix[3][1];
        this->_planes[5].normal.x = -this->_matrix[0][3] - this->_matrix[0][1];
        this->_planes[5].normal.y = -this->_matrix[1][3] - this->_matrix[1][1];
        this->_planes[5].normal.z = -this->_matrix[2][3] - this->_matrix[2][1];
        this->_planes[5].d = -this->_matrix[3][3] - this->_matrix[3][1];
        this->_planes[0].normal.x = -this->_matrix[0][2];
        this->_planes[0].normal.y = -this->_matrix[1][2];
        this->_planes[0].normal.z = -this->_matrix[2][2];
        this->_planes[0].d = -this->_matrix[3][2];
        this->_planes[1].normal.x = -this->_matrix[0][3] + this->_matrix[0][2];
        this->_planes[1].normal.y = -this->_matrix[1][3] + this->_matrix[1][2];
        this->_planes[1].normal.z = -this->_matrix[2][3] + this->_matrix[2][2];
        this->_planes[1].d = -this->_matrix[3][3] + this->_matrix[3][2];
        for (int i = 0; i < 6; i++)
        {
            double mag = glm::length(this->_planes[i].normal);
            this->_planes[i].normal /= mag;
            this->_planes[i].d /= mag;
        }

        Ray ray = this->_planes[0].GetIntersection(this->_planes[2]);
        this->_corners[0] = this->_planes[4].GetIntersection(ray);
        this->_corners[3] = this->_planes[5].GetIntersection(ray);
        ray = this->_planes[3].GetIntersection(this->_planes[0]);
        this->_corners[1] = this->_planes[4].GetIntersection(ray);
        this->_corners[2] = this->_planes[5].GetIntersection(ray);
        ray = this->_planes[2].GetIntersection(this->_planes[1]);
        this->_corners[4] = this->_planes[4].GetIntersection(ray);
        this->_corners[7] = this->_planes[5].GetIntersection(ray);
        ray = this->_planes[1].GetIntersection(this->_planes[3]);
        this->_corners[5] = this->_planes[4].GetIntersection(ray);
        this->_corners[6] = this->_planes[5].GetIntersection(ray);

        this->_center = this->_corners[0];
        for (int i = 1; i < 8; ++i)
            this->_center += this->_corners[i];
        this->_center /= 8;
        this->_radiusSquared = 0;
        for (int i = 0; i < 8; ++i)
            this->_radiusSquared = std::max(this->_radiusSquared, glm::distanceSquared(this->_corners[i], this->_center));
    }

    glm::dvec3 const& Frustum::SupportMapping(glm::dvec3 const& v) const
    {
        int num = 0;
        double num2 = glm::dot(this->_corners[0], v);
        for (int i = 1; i < 8; i++)
        {
            double num3 = glm::dot(this->_corners[i], v);
            if (num3 > num2)
            {
                num = i;
                num2 = num3;
            }
        }
        return this->_corners[num];
    }

    AbstractCollider::IntersectionType Frustum::Contains(glm::dvec3 const& point) const
    {
        if (glm::distanceSquared(point, this->_center) > this->_radiusSquared)
            return Outside;
        for (int i = 0; i < 6; i++)
            if (glm::dot(this->_planes[i].normal, point) + this->_planes[i].d > 1E-05)
                return Outside;
        return Inside;
    }

    AbstractCollider::IntersectionType Frustum::Contains(AlignedBox const& box) const
    {
        if (glm::distance(box.GetCenter(), this->_center) > std::sqrt(this->_radiusSquared) + std::sqrt(box.GetRadiusSquared()))
            return Outside;
        bool flag = false;
        for (int i = 0; i < 6; i++)
        {
            switch (box.Intersects(this->_planes[i]))
            {
            case Plane::Front:
                return Outside;

            case Plane::Intersecting:
                flag = true;
                break;

            case Plane::Back:
                break;
            }
        }
        return flag ? Intersecting : Inside;
    }

    AbstractCollider::IntersectionType Frustum::Contains(AlignedCube const& box) const
    {
        bool flag = false;
        for (int i = 0; i < 6; i++)
        {
            switch (box.Intersects(this->_planes[i]))
            {
            case Plane::Front:
                return Outside;

            case Plane::Intersecting:
                flag = true;
                break;

            case Plane::Back:
                break;
            }
        }
        return flag ? Intersecting : Inside;
    }

    AbstractCollider::IntersectionType Frustum::Contains(Frustum const& frustum) const
    {
        IntersectionType disjoint = Outside;
        if (this->Intersects(frustum))
        {
            disjoint = Inside;
            for (int i = 0; i < 8; i++)
                if (!this->Contains(frustum._corners[i]))
                    return Intersecting;
        }
        return disjoint;
    }

}
