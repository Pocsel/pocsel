#include <luasel/Luasel.hpp>

#include "common/physics/ShapeDescEmpty.hpp"

#include "tools/ByteArray.hpp"

namespace Common { namespace Physics {

    ShapeDescEmpty::ShapeDescEmpty(Luasel::Ref const&)
    {
    }

    ShapeDescEmpty::~ShapeDescEmpty()
    {
    }

    std::string const& ShapeDescEmpty::ToString() const
    {
        static std::string ret("empty");
        return ret;
    }

    std::unique_ptr<btCollisionShape> ShapeDescEmpty::CreateShape() const
    {
        return std::unique_ptr<btCollisionShape>(new btEmptyShape());
    }

    void ShapeDescEmpty::Serialize(Tools::ByteArray& byteArray) const
    {
        byteArray.Write8((Uint8)ShapeType::Empty);
    }

    ShapeDescEmpty::ShapeDescEmpty(Tools::ByteArray const&)
    {
    }


}}
