#ifndef __COMMON_PHYSICS_SHAPEDESCSPHERE_HPP__
#define __COMMON_PHYSICS_SHAPEDESCSPHERE_HPP__

#include "common/physics/ShapeDesc.hpp"

#include "bullet/bullet-all.hpp"

namespace Common { namespace Physics {

    class ShapeDescSphere :
        public ShapeDesc
    {
    private:
        btScalar _radius;

    public:
        ShapeDescSphere(Tools::Lua::Ref const& lua);
        ShapeDescSphere(Tools::ByteArray const& byteArray);
        virtual ~ShapeDescSphere();

        virtual std::string const& GetName() const;
        virtual std::unique_ptr<btCollisionShape> CreateShape() const;
        virtual void Serialize(Tools::ByteArray& byteArray) const;
    };

}}

#endif
