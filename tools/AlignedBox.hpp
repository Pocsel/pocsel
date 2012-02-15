#ifndef __TOOLS_ALIGNEDBOX_HPP__
#define __TOOLS_ALIGNEDBOX_HPP__

#include "tools/AbstractCollider.hpp"
#include "tools/Vector3.hpp"
#include "tools/Plane.hpp"

namespace Tools {

    class AlignedBox : public AbstractCollider
    {
    private:
        Vector3d _min;
        Vector3d _max;

    public:
        AlignedBox(Vector3d const& point1, Vector3d const& point2) :
            _min(point1),
            _max(point2)
        {
            this->_Update();
        }

        Vector3d const& GetMin() const { return this->_min; }
        Vector3d const& GetMax() const { return this->_max; }
        void SetMin(Vector3d const& min) { this->_min = min; this->_Update(); }
        void SetMax(Vector3d const& max) { this->_max = max; this->_Update(); }

        Plane::IntersectionType Intersects(Plane const& plane) const;

        virtual IntersectionType Contains(Vector3d const& object) const;
        virtual IntersectionType Contains(AlignedBox const& object) const;
        virtual IntersectionType Contains(AlignedCube const& object) const;
        virtual IntersectionType Contains(Frustum const& object) const;

        Vector3d SupportMapping(Vector3d const& v) const;

    private:
        void _Update()
        {
            Vector3d min(this->_min);
            this->_min.x = std::min(min.x, this->_max.x);
            this->_min.y = std::min(min.y, this->_max.y);
            this->_min.z = std::min(min.z, this->_max.z);
            this->_max.x = std::max(min.x, this->_max.x);
            this->_max.y = std::max(min.y, this->_max.y);
            this->_max.z = std::max(min.z, this->_max.z);
        }
    };

}

#endif
