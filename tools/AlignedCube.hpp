#ifndef __TOOLS_ALIGNEDCUBE_HPP__
#define __TOOLS_ALIGNEDCUBE_HPP__

#include "tools/AbstractCollider.hpp"
#include "tools/Vector3.hpp"
#include "tools/Plane.hpp"

namespace Tools {

    class AlignedCube : public AbstractCollider
    {
    private:
        glm::dvec3 _position;
        double _size;

    public:
        AlignedCube(glm::dvec3 const& position, double size)
            : _position(position),
            _size(size)
        {
        }

        glm::dvec3 const& GetPosition() const { return this->_position; }
        double GetSize() const { return this->_size; }
        void SetPosition(glm::dvec3 const& position) { this->_position = position; }
        void SetSize(double size) { this->_size = size; }

        Plane::IntersectionType Intersects(Plane const& plane) const;

        virtual IntersectionType Contains(glm::dvec3 const& object) const;
        virtual IntersectionType Contains(AlignedBox const& object) const;
        virtual IntersectionType Contains(AlignedCube const& object) const;
        virtual IntersectionType Contains(Frustum const& object) const;

        glm::dvec3 SupportMapping(glm::dvec3 const& v) const;
    };

}

#endif
