#include "common/physics/ShapeDescBox.hpp"

#include "tools/lua/Ref.hpp"
#include "tools/ByteArray.hpp"

namespace Common { namespace Physics {

    ShapeDescBox::ShapeDescBox(Tools::Lua::Ref const& lua)
    {
        Tools::Lua::Ref halfExtentsLua = lua["halfExtents"];
        if (!halfExtentsLua.IsTable())
            throw std::runtime_error("Field \"halfExtents\" must be of type table");
        _halfExtents.setX(halfExtentsLua[1].ToNumber());
        _halfExtents.setY(halfExtentsLua[2].ToNumber());
        _halfExtents.setZ(halfExtentsLua[3].ToNumber());
    }

    ShapeDescBox::~ShapeDescBox()
    {
    }

    std::unique_ptr<btCollisionShape> ShapeDescBox::CreateShape() const
    {
        return std::unique_ptr<btCollisionShape>(new btBoxShape(this->_halfExtents));
    }

    void ShapeDescBox::Serialize(Tools::ByteArray& byteArray) const
    {
        byteArray.Write8((Uint8)ShapeType::Box);
        byteArray.Write(_halfExtents.x());
        byteArray.Write(_halfExtents.y());
        byteArray.Write(_halfExtents.z());
    }

    ShapeDescBox::ShapeDescBox(Tools::ByteArray const& byteArray) :
        _halfExtents(byteArray.ReadDouble(), byteArray.ReadDouble(), byteArray.ReadDouble())
    {
    }

}}