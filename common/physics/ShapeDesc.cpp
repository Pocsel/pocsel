#include "common/physics/ShapeDesc.hpp"
#include "common/physics/ShapeDescBox.hpp"
#include "common/physics/ShapeDescSphere.hpp"
#include "common/physics/ShapeDescEmpty.hpp"

#include "tools/lua/Ref.hpp"
#include "tools/lua/Function.hpp"
#include "tools/ByteArray.hpp"

namespace Common { namespace Physics {

    std::unique_ptr<ShapeDesc> ShapeDesc::BuildShapeDesc(Tools::Lua::Ref const& lua)
    {
        Tools::Lua::Ref shapeTypeLua = lua["shapeType"];
        if (!shapeTypeLua.IsString())
            throw std::runtime_error("Field \"shapeType\" must be of type string");
        std::string shapeType = shapeTypeLua.ToString();

        if (shapeType == "empty")
            return std::unique_ptr<ShapeDesc>(new ShapeDescEmpty(lua));
        if (shapeType == "sphere")
            return std::unique_ptr<ShapeDesc>(new ShapeDescSphere(lua));
        if (shapeType == "box")
            return std::unique_ptr<ShapeDesc>(new ShapeDescBox(lua));

        throw std::runtime_error("you need a correct shapeType");
    }

    std::unique_ptr<ShapeDesc> ShapeDesc::DeSerialize(Tools::ByteArray const& byteArray)
    {
        ShapeType type = (ShapeType)byteArray.Read8();

        if (type == ShapeType::Empty)
            return std::unique_ptr<ShapeDesc>(new ShapeDescEmpty(byteArray));
        if (type == ShapeType::Sphere)
            return std::unique_ptr<ShapeDesc>(new ShapeDescSphere(byteArray));
        if (type == ShapeType::Box)
            return std::unique_ptr<ShapeDesc>(new ShapeDescBox(byteArray));

        throw std::runtime_error("Unknown shape type: " + Tools::ToString((Uint32)type));
    }

}}
