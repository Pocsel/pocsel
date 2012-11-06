#ifndef __COMMON_PHYSICS_SHAPEDESCBOX_HPP__
#define __COMMON_PHYSICS_SHAPEDESCBOX_HPP__

#include "common/physics/IShapeDesc.hpp"

#include "bullet/bullet-all.hpp"

namespace Common { namespace Physics {

    class ShapeDescBox :
        public IShapeDesc
    {
    private:
        btVector3 _halfExtents;
        mutable std::unique_ptr<std::string> _string;

    public:
        ShapeDescBox(Luasel::Ref const& lua);
        ShapeDescBox(Tools::ByteArray const& byteArray);
        virtual ~ShapeDescBox();

        virtual std::string const& ToString() const;
        virtual std::unique_ptr<btCollisionShape> CreateShape() const;
        virtual void Serialize(Tools::ByteArray& byteArray) const;
    };

}}

#endif
