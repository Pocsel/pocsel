#ifndef __COMMON_PHYSICS_SHAPEDESCBOX_HPP__
#define __COMMON_PHYSICS_SHAPEDESCBOX_HPP__

#include "common/physics/ShapeDesc.hpp"

#include "bullet/bullet-all.hpp"

namespace Common { namespace Physics {

    class ShapeDescBox :
        public ShapeDesc
    {
    private:
        btVector3 _halfExtents;

    public:
        ShapeDescBox(Tools::Lua::Ref const& lua);
        ShapeDescBox(Tools::ByteArray const& byteArray);
        virtual ~ShapeDescBox();

        virtual std::string const& GetName() const;
        virtual std::unique_ptr<btCollisionShape> CreateShape() const;
        virtual void Serialize(Tools::ByteArray& byteArray) const;
    };

}}

#endif
