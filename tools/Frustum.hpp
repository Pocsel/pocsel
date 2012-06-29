#ifndef __TOOLS_FRUSTUM_HPP__
#define __TOOLS_FRUSTUM_HPP__

#include "tools/AbstractCollider.hpp"
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
        glm::detail::tmat4x4<double> _matrix;
        glm::dvec3 _corners[8];
        glm::dvec3 _center;
        double _radiusSquared;

    public:
        Frustum(glm::detail::tmat4x4<double> const& view);

        glm::dvec3 const* GetCorners() const { return this->_corners; }
        glm::dvec3 const& GetCenter() const { return this->_center; }
        double GetRadius() const { return std::sqrt(this->_radiusSquared); }

        virtual IntersectionType Contains(glm::dvec3 const& object) const;
        virtual IntersectionType Contains(AlignedBox const& object) const;
        virtual IntersectionType Contains(AlignedCube const& object) const;
        virtual IntersectionType Contains(Frustum const& object) const;

        glm::dvec3 const& SupportMapping(glm::dvec3 const& v) const;

    private:
        Ray _ComputeIntersectionLine() const;
    };

}

#endif
