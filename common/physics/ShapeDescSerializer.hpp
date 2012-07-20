#ifndef __COMMON_PHYSICS_ShapeDescSERIALIZER_HPP__
#define __COMMON_PHYSICS_ShapeDescSERIALIZER_HPP__

#include "tools/ByteArray.hpp"

#include "common/physics/ShapeDesc.hpp"

namespace Tools {

    template<> struct ByteArray::Serializer< Common::Physics::ShapeDesc >
    {

        static void Write(Common::Physics::ShapeDesc const& sd, ByteArray& p) // Used by Packet::Write<T>(T const&)
        {
            sd.Serialize(p);
        }

        static std::unique_ptr<Common::Physics::ShapeDesc> Read(ByteArray const& p) // Used by Packet::Read<T>()
        {
            return Common::Physics::ShapeDesc::DeSerialize(p);
        }


        private:
        static void Read(ByteArray const& p, Common::Physics::ShapeDesc& sn);  // Used by Packet::Read<T>(T&)
    };

}

#endif

