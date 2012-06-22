#ifndef __COMMON_PHYSICS_NODESERIALIZER_HPP__
#define __COMMON_PHYSICS_NODESERIALIZER_HPP__

#include "tools/ByteArray.hpp"
#include "tools/QuaternionSerializer.hpp"

#include "common/physics/Node.hpp"

namespace Tools {

    // Packet
    template<> struct ByteArray::Serializer<Common::Physics::Node>
    {
        static void Read(ByteArray const& p, Common::Physics::Node& v)  // Used by Packet::Read<T>(T&)
        {
            p.Read(v.position);
            p.Read(v.velocity);
            p.Read(v.acceleration);

            p.Read(v.orientation);
            p.Read(v.angularVelocity);
            //p.Read(v.angularAcceleration);

            //p.Read(v.scale);
            //p.Read(v.scaleVelocity);
            //p.Read(v.scaleAcceleration);
        }

        static void Write(Common::Physics::Node const& v, ByteArray& p) // Used by Packet::Write<T>(T const&)
        {
            p.Write(v.position);
            p.Write(v.velocity);
            p.Write(v.acceleration);

            p.Write(v.orientation);
            p.Write(v.angularVelocity);
            //p.Write(v.angularAcceleration);

            //p.Write(v.scale);
            //p.Write(v.scaleVelocity);
            //p.Write(v.scaleAcceleration);
        }

        static std::unique_ptr<Common::Physics::Node> Read(ByteArray const& p) // Used by Packet::Read<T>()
        {
            std::unique_ptr<Common::Physics::Node> v(new Common::Physics::Node());

            p.Read(v->position);
            p.Read(v->velocity);
            p.Read(v->acceleration);

            p.Read(v->orientation);
            p.Read(v->angularVelocity);
            //p.Read(v->angularAcceleration);

            //p.Read(v->scale);
            //p.Read(v->scaleVelocity);
            //p.Read(v->scaleAcceleration);

            return v;
        }
    };

}

#endif
