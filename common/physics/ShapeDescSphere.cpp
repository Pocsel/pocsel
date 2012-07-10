#include "common/physics/ShapeDescSphere.hpp"

#include "tools/lua/Ref.hpp"
#include "tools/ByteArray.hpp"

namespace Common { namespace Physics {

    ShapeDescSphere::ShapeDescSphere(Tools::Lua::Ref const& lua)
    {
        Tools::Lua::Ref radiusLua = lua["radius"];
        if (!radiusLua.IsNumber())
            throw std::runtime_error("Field \"radius\" must be of type number");
        _radius = radiusLua.ToNumber();
    }

    ShapeDescSphere::~ShapeDescSphere()
    {
    }

    std::string const& ShapeDescSphere::GetName() const
    {
        static std::string ret("sphere");
        return ret;
    }

    std::unique_ptr<btCollisionShape> ShapeDescSphere::CreateShape() const
    {
        return std::unique_ptr<btCollisionShape>(new btSphereShape(this->_radius));
    }

    void ShapeDescSphere::Serialize(Tools::ByteArray& byteArray) const
    {
        byteArray.Write8((Uint8)ShapeType::Sphere);
        byteArray.Write(_radius);
    }

    ShapeDescSphere::ShapeDescSphere(Tools::ByteArray const& byteArray) :
        _radius(byteArray.ReadDouble())
    {
    }

}}
