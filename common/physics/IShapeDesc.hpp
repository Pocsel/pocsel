#ifndef __COMMON_PHYSICS_ISHAPEDESC_HPP__
#define __COMMON_PHYSICS_ISHAPEDESC_HPP__

#include "tools/enum.hpp"

namespace Tools {
    class ByteArray;
    namespace Lua {
        class Ref;
    }
}

class btCollisionShape;

namespace Common { namespace Physics {

    class IShapeDesc :
        private boost::noncopyable
    {
    protected:
        ENUM ShapeType
        {
            Empty = 0,
            Sphere = 1,
            Box = 2
        };
    public:
        static std::unique_ptr<IShapeDesc> BuildShapeDesc(Tools::Lua::Ref const& lua);
        static std::unique_ptr<IShapeDesc> DeSerialize(Tools::ByteArray const& byteArray);

    public:
        virtual std::string const& ToString() const = 0;
        virtual std::unique_ptr<btCollisionShape> CreateShape() const = 0;
        virtual void Serialize(Tools::ByteArray& byteArray) const = 0;
        virtual ~IShapeDesc() {}
    };

}}

#endif
