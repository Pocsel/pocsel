#ifndef __TOOLS_FRUSTUM_HPP__
#define __TOOLS_FRUSTUM_HPP__

#include "tools/AbstractCollider.hpp"
#include "tools/Matrix4.hpp"
#include "tools/Plane.hpp"
#include "tools/Vector3.hpp"

namespace Tools {

    class Frustum : public AbstractCollider
    {
    private:
        enum
        {
            NearPlane = 0,
            FarPlane,
            LeftPlane,
            RightPlane,
            TopPlane,
            BottomPlane
        };

        Plane _planes[6];
        Matrix4<double> _matrix;
        Vector3d _corners[8];

    public:
        Frustum(Matrix4<double> const& view);

        Vector3d const* GetCorners() const { return this->_corners; }

        virtual IntersectionType Contains(Vector3d const& object) const;
        virtual IntersectionType Contains(AlignedBox const& object) const;
        virtual IntersectionType Contains(Frustum const& object) const;

        Vector3d const& SupportMapping(Vector3d const& v) const;

    private:
        Ray _ComputeIntersectionLine() const;
    };

}

#endif
