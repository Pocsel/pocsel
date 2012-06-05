#ifndef __TOOLS_PHYSICSOBJECTSERIALIZER_HPP__
#define __TOOLS_PHYSICSOBJECTSERIALIZER_HPP__

#include "tools/ByteArray.hpp"
#include "tools/PhysicsObject.hpp"

namespace Tools {

    // Packet
    template<typename T> struct ByteArray::Serializer<PhysicsObject>
    {
        static void Read(ByteArray const& p, PhysicsObject& v)  // Used by Packet::Read<T>(T&)
        {
            p.Write(v.position);
        }

        static void Write(PhysicsObject const& v, ByteArray& p) // Used by Packet::Write<T>(T const&)
        {
        }

        static std::unique_ptr<PhysicsObject> Read(ByteArray const& p) // Used by Packet::Read<T>()
        {
        }
    };

}

#endif


