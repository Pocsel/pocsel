#ifndef __COMMON_PHYSICS_NODESERIALIZER_HPP__
#define __COMMON_PHYSICS_NODESERIALIZER_HPP__

#include "tools/ByteArray.hpp"

#include "common/physics/Node.hpp"

namespace Tools {

    // Packet
    template struct ByteArray::Serializer<Common::Physics::Node>
    {
        static void Read(ByteArray const& p, Common::Physics::Node& v)  // Used by Packet::Read<T>(T&)
        {
            p.Read(v.position.r);
            p.Read(v.position.v);
            p.Read(v.position.a);

            p.Read(v.yawPitchRoll.r);
            p.Read(v.yawPitchRoll.v);
            p.Read(v.yawPitchRoll.a);

            p.Read(v.scale.r);
            p.Read(v.scale.v);
            p.Read(v.scale.a);
        }

        static void Write(PhysicsObject const& v, ByteArray& p) // Used by Packet::Write<T>(T const&)
        {
            p.Write(v.position.r);
            p.Write(v.position.v);
            p.Write(v.position.a);

            p.Write(v.yawPitchRoll.r);
            p.Write(v.yawPitchRoll.v);
            p.Write(v.yawPitchRoll.a);

            p.Write(v.scale.r);
            p.Write(v.scale.v);
            p.Write(v.scale.a);
        }

        static std::unique_ptr<PhysicsObject> Read(ByteArray const& p) // Used by Packet::Read<T>()
        {
            std::unique_ptr<Common::Physics::Node> v(new Common::Physics::Node());

            p.Read(v.position.r);
            p.Read(v.position.v);
            p.Read(v.position.a);

            p.Read(v.yawPitchRoll.r);
            p.Read(v.yawPitchRoll.v);
            p.Read(v.yawPitchRoll.a);

            p.Read(v.scale.r);
            p.Read(v.scale.v);
            p.Read(v.scale.a);

            return v;
        }
    };

}

#endif
