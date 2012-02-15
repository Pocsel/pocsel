#ifndef __TOOLS_ALIGNEDCUBE_HPP__
#define __TOOLS_ALIGNEDCUBE_HPP__

#include "tools/AbstractCollider.hpp"
#include "tools/Vector3.hpp"
#include "tools/Plane.hpp"

namespace Tools {

    class AlignedCube : public AbstractCollider
    {
    private:
        Vector3d _position;
        double _size;

    public:
        AlignedCube(Vector3d const& position, double size)
            : _position(position),
            _size(size)
        {
        }

        Vector3d const& GetPosition() const { return this->_position; }
        double GetSize() const { return this->_size; }
        void SetPosition(Vector3d const& position) { this->_position = position; }
        void SetSize(double size) { this->_size = size; }

        Plane::IntersectionType Intersects(Plane const& plane) const;

        virtual IntersectionType Contains(Vector3d const& object) const;
        virtual IntersectionType Contains(AlignedBox const& object) const;
        virtual IntersectionType Contains(AlignedCube const& object) const;
        virtual IntersectionType Contains(Frustum const& object) const;

        Vector3d SupportMapping(Vector3d const& v) const;
    };

}

#endif
