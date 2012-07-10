#include "common/physics/ShapeDescEmpty.hpp"

#include "tools/lua/Ref.hpp"
#include "tools/ByteArray.hpp"

namespace Common { namespace Physics {

    ShapeDescEmpty::ShapeDescEmpty(Tools::Lua::Ref const&)
    {
    }

    ShapeDescEmpty::~ShapeDescEmpty()
    {
    }

    std::string const& ShapeDescEmpty::GetName() const
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
