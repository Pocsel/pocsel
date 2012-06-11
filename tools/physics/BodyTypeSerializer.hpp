#ifndef __TOOLS_PHYSICS_BODYTYPESERIALIZER_HPP__
#define __TOOLS_PHYSICS_BODYTYPESERIALIZER_HPP__

#include "tools/ByteArray.hpp"
#include "tools/StdVectorSerializer.hpp"

#include "tools/physics/BodyType.hpp"

namespace Tools {

    namespace Physics {

        template<typename TBodyType>
            struct BodyTypeSerializer
            {

                static void Write(TBodyType const& bt, ByteArray& p) // Used by Packet::Write<T>(T const&)
                {
                    p.Write(bt.GetShapes());
                }

                static std::unique_ptr<TBodyType> Read(ByteArray const& p) // Used by Packet::Read<T>()
                {
                    std::vector<BodyType::ShapeNode> vec;
                    p.Read(vec);
                    return std::unique_ptr<TBodyType>(new TBodyType(vec));
                }

                private:
                static void Read(ByteArray const& p, TBodyType& bt);  // Used by Packet::Read<T>(T&)
            };
    }

    template<> struct ByteArray::Serializer< Tools::Physics::BodyType::ShapeNode >
    {

        static void Write(Tools::Physics::BodyType::ShapeNode const& sn, ByteArray& p) // Used by Packet::Write<T>(T const&)
        {
            p.Write(sn.name);
            p.Write(sn.children);
            p.Write(sn.parent);
        }

        static void Read(ByteArray const& p, Tools::Physics::BodyType::ShapeNode& sn)  // Used by Packet::Read<T>(T&)
        {
            p.Read(sn.name);
            p.Read(sn.children);
            p.Read(sn.parent);
        }

        private:
        static std::unique_ptr<Tools::Physics::BodyType::ShapeNode> Read(ByteArray const& p); // Used by Packet::Read<T>()
    };

}

#endif
