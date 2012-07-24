#ifndef __COMMON_PHYSICS_SHAPEDESCSPHERE_HPP__
#define __COMMON_PHYSICS_SHAPEDESCSPHERE_HPP__

#include "common/physics/IShapeDesc.hpp"

#include "bullet/bullet-all.hpp"

namespace Common { namespace Physics {

    class ShapeDescSphere :
        public IShapeDesc
    {
    private:
        btScalar _radius;
        mutable std::unique_ptr<std::string> _string;

    public:
        ShapeDescSphere(Tools::Lua::Ref const& lua);
        ShapeDescSphere(Tools::ByteArray const& byteArray);
        virtual ~ShapeDescSphere();

        virtual std::string const& ToString() const;
        virtual std::unique_ptr<btCollisionShape> CreateShape() const;
        virtual void Serialize(Tools::ByteArray& byteArray) const;
    };

}}

#endif
