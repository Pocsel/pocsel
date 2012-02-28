#include "tools/AlignedBox.hpp"
#include "tools/AlignedCube.hpp"
#include "tools/Frustum.hpp"
#include "tools/Gjk.hpp"
#include "tools/Ray.hpp"

namespace Tools {

    Frustum::Frustum(Matrix4<double> const& view)
    {
        this->_matrix = view;
        this->_planes[2].normal.x = -view.m[0][3] - view.m[0][0];
        this->_planes[2].normal.y = -view.m[1][3] - view.m[1][0];
        this->_planes[2].normal.z = -view.m[2][3] - view.m[2][0];
        this->_planes[2].d = -view.m[3][3] - view.m[3][0];
        this->_planes[3].normal.x = -view.m[0][3] + view.m[0][0];
        this->_planes[3].normal.y = -view.m[1][3] + view.m[1][0];
        this->_planes[3].normal.z = -view.m[2][3] + view.m[2][0];
        this->_planes[3].d = -view.m[3][3] + view.m[3][0];
        this->_planes[4].normal.x = -view.m[0][3] + view.m[0][1];
        this->_planes[4].normal.y = -view.m[1][3] + view.m[1][1];
        this->_planes[4].normal.z = -view.m[2][3] + view.m[2][1];
        this->_planes[4].d = -view.m[3][3] + view.m[3][1];
        this->_planes[5].normal.x = -view.m[0][3] - view.m[0][1];
        this->_planes[5].normal.y = -view.m[1][3] - view.m[1][1];
        this->_planes[5].normal.z = -view.m[2][3] - view.m[2][1];
        this->_planes[5].d = -view.m[3][3] - view.m[3][1];
        this->_planes[0].normal.x = -view.m[0][2];
        this->_planes[0].normal.y = -view.m[1][2];
        this->_planes[0].normal.z = -view.m[2][2];
        this->_planes[0].d = -view.m[3][2];
        this->_planes[1].normal.x = -view.m[0][3] + view.m[0][2];
        this->_planes[1].normal.y = -view.m[1][3] + view.m[1][2];
        this->_planes[1].normal.z = -view.m[2][3] + view.m[2][2];
        this->_planes[1].d = -view.m[3][3] + view.m[3][2];
        for (int i = 0; i < 6; i++)
        {
            double mag = this->_planes[i].normal.GetMagnitude();
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
            this->_radiusSquared = std::max(this->_radiusSquared, Vector3d::GetDistanceSquared(this->_corners[i], this->_center));
    }

    Vector3d const& Frustum::SupportMapping(Vector3d const& v) const
    {
        int num = 0;
        double num2 = Vector3d::Dot(this->_corners[0], v);
        for (int i = 1; i < 8; i++)
        {
            double num3 = Vector3d::Dot(this->_corners[i], v);
            if (num3 > num2)
            {
                num = i;
                num2 = num3;
            }
        }
        return this->_corners[num];
    }

    AbstractCollider::IntersectionType Frustum::Contains(Vector3d const& point) const
    {
        if (Vector3d::GetDistanceSquared(point, this->_center) > this->_radiusSquared)
            return Outside;
        for (int i = 0; i < 6; i++)
            if (Vector3d::Dot(this->_planes[i].normal, point) + this->_planes[i].d > 1E-05)
                return Outside;
        return Inside;
    }

    AbstractCollider::IntersectionType Frustum::Contains(AlignedBox const& box) const
    {
        if (Vector3d::GetDistance(box.GetCenter(), this->_center) > std::sqrt(this->_radiusSquared) + std::sqrt(box.GetRadiusSquared()))
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
