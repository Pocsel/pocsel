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

            p.Read(v.orientation);
            p.Read(v.angularVelocity);

            p.Read(v.accelerationIsLocal);
            p.Read(v.acceleration);
            p.Read(v.maxSpeed);

            p.Read(v.interPosition);
            p.Read(v.interAngle);
            p.Read(v.interPositionTarget);
            p.Read(v.interPositionTargetSpeed);
            p.Read(v.interAngleTarget);
            p.Read(v.interAngleTargetSpeed);
        }

        static void Write(Common::Physics::Node const& v, ByteArray& p) // Used by Packet::Write<T>(T const&)
        {
            p.Write(v.position);
            p.Write(v.velocity);

            p.Write(v.orientation);
            p.Write(v.angularVelocity);

            p.Write(v.accelerationIsLocal);
            p.Write(v.acceleration);
            p.Write(v.maxSpeed);

            p.Write(v.interPosition);
            p.Write(v.interAngle);
            p.Write(v.interPositionTarget);
            p.Write(v.interPositionTargetSpeed);
            p.Write(v.interAngleTarget);
            p.Write(v.interAngleTargetSpeed);
        }

        static std::unique_ptr<Common::Physics::Node> Read(ByteArray const& p) // Used by Packet::Read<T>()
        {
            std::unique_ptr<Common::Physics::Node> v(new Common::Physics::Node());

            p.Read(v->position);
            p.Read(v->velocity);

            p.Read(v->orientation);
            p.Read(v->angularVelocity);

            p.Read(v->accelerationIsLocal);
            p.Read(v->acceleration);
            p.Read(v->maxSpeed);

            p.Read(v->interPosition);
            p.Read(v->interAngle);
            p.Read(v->interPositionTarget);
            p.Read(v->interPositionTargetSpeed);
            p.Read(v->interAngleTarget);
            p.Read(v->interAngleTargetSpeed);

            return v;
        }
    };

}

#endif
