#ifndef __TOOLS_ALIGNEDBOX_HPP__
#define __TOOLS_ALIGNEDBOX_HPP__

#include "tools/AbstractCollider.hpp"
#include "tools/Vector3.hpp"
#include "tools/Plane.hpp"

namespace Tools {

    class AlignedBox : public AbstractCollider
    {
    private:
        glm::dvec3 _min;
        glm::dvec3 _max;

    public:
        AlignedBox(glm::dvec3 const& point1, glm::dvec3 const& point2) :
            _min(point1),
            _max(point2)
        {
            this->_Update();
        }
        virtual ~AlignedBox() {}

        glm::dvec3 const& GetMin() const { return this->_min; }
        glm::dvec3 const& GetMax() const { return this->_max; }
        glm::dvec3 GetCenter() const { return (this->_max - this->_min) * 0.5 + this->_min; }
        double GetRadius() const { return glm::distance(this->GetCenter(), this->_min); }
        double GetRadiusSquared() const { return glm::distanceSquared(this->GetCenter(), this->_min); }
        void SetMin(glm::dvec3 const& min) { this->_min = min; this->_Update(); }
        void SetMax(glm::dvec3 const& max) { this->_max = max; this->_Update(); }

        Plane::IntersectionType Intersects(Plane const& plane) const;

        virtual IntersectionType Contains(glm::dvec3 const& object) const;
        virtual IntersectionType Contains(AlignedBox const& object) const;
        virtual IntersectionType Contains(AlignedCube const& object) const;
        virtual IntersectionType Contains(Frustum const& object) const;

        glm::dvec3 SupportMapping(glm::dvec3 const& v) const;

    private:
        void _Update()
        {
            glm::dvec3 min(this->_min);
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
