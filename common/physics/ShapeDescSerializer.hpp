#ifndef __COMMON_PHYSICS_SHAPEDESCSERIALIZER_HPP__
#define __COMMON_PHYSICS_SHAPEDESCSERIALIZER_HPP__

#include "tools/ByteArray.hpp"

#include "common/physics/IShapeDesc.hpp"

namespace Tools {

    template<> struct ByteArray::Serializer< Common::Physics::IShapeDesc >
    {

        static void Write(Common::Physics::IShapeDesc const& sd, ByteArray& p) // Used by Packet::Write<T>(T const&)
        {
            sd.Serialize(p);
        }

        static std::unique_ptr<Common::Physics::IShapeDesc> Read(ByteArray const& p) // Used by Packet::Read<T>()
        {
            return Common::Physics::IShapeDesc::DeSerialize(p);
        }


        private:
        static void Read(ByteArray const& p, Common::Physics::IShapeDesc& sn);  // Used by Packet::Read<T>(T&)
    };

}

#endif

