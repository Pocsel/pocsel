#ifndef __TOOLS_PHYSICS_BODYTYPESERIALIZER_HPP__
#define __TOOLS_PHYSICS_BODYTYPESERIALIZER_HPP__

#include "tools/ByteArray.hpp"
#include "tools/StdVectorSerializer.hpp"

#include "tools/physics/BodyType.hpp"

namespace Tools {
    
    namespace Physics {

        template<typename BodyTypeType>
            struct BodyTypeSerializer
            {

                static void Write(BodyTypeType const& bt, ByteArray& p) // Used by Packet::Write<T>(T const&)
                {
                    p.Write(bt.GetShapes());
                }

                static std::unique_ptr<BodyTypeType> Read(ByteArray const& p) // Used by Packet::Read<T>()
                {
                    std::vector<Tools::Physics::BodyType::ShapeNode> vec;
                    p.Read(vec);
                    return std::unique_ptr<BodyTypeType>(new BodyTypeType(vec));
                }

                private:
                static void Read(ByteArray const& p, BodyTypeType& bt);  // Used by Packet::Read<T>(T&)
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
