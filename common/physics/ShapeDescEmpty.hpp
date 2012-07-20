#ifndef __COMMON_PHYSICS_SHAPEDESCEMPTY_HPP__
#define __COMMON_PHYSICS_SHAPEDESCEMPTY_HPP__

#include "common/physics/IShapeDesc.hpp"

#include "bullet/bullet-all.hpp"

namespace Common { namespace Physics {

    class ShapeDescEmpty :
        public IShapeDesc
    {
    public:
        ShapeDescEmpty(Tools::Lua::Ref const& lua);
        ShapeDescEmpty(Tools::ByteArray const& byteArray);
        virtual ~ShapeDescEmpty();

        virtual std::string const& ToString() const;
        virtual std::unique_ptr<btCollisionShape> CreateShape() const;
        virtual void Serialize(Tools::ByteArray& byteArray) const;
    };

}}

#endif
