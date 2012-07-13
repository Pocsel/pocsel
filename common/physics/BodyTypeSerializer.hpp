#ifndef __COMMON_PHYSICS_BODYTYPESERIALIZER_HPP__
#define __COMMON_PHYSICS_BODYTYPESERIALIZER_HPP__

#include "tools/ByteArray.hpp"
#include "tools/StdVectorSerializer.hpp"

#include "common/physics/BodyType.hpp"
#include "common/physics/ShapeDescSerializer.hpp"

namespace Common { namespace Physics {

    template<typename TBodyType>
        struct BodyTypeSerializer
        {

            static void Write(TBodyType const& bt, Tools::ByteArray& p) // Used by Packet::Write<T>(T const&)
            {
                p.Write(bt.GetShapes());
            }

            static std::unique_ptr<TBodyType> Read(Tools::ByteArray const& p) // Used by Packet::Read<T>()
            {
                std::vector<BodyType::ShapeNode> vec;
                p.Read(vec);
                std::unique_ptr<TBodyType> ret(new TBodyType(vec));
                ret->CreateBtShapes();
                return ret;
            }

            private:
            static void Read(Tools::ByteArray const& p, TBodyType& bt);  // Used by Packet::Read<T>(T&)
        };
}}

namespace Tools {

    template<> struct ByteArray::Serializer< Common::Physics::BodyType::ShapeNode >
    {

        static void Write(Common::Physics::BodyType::ShapeNode const& sn, ByteArray& p) // Used by Packet::Write<T>(T const&)
        {
            p.Write(sn.name);
            p.Write(sn.children);
            p.Write(sn.parent);
            p.Write(*sn.shapeDesc);
        }

        static void Read(ByteArray const& p, Common::Physics::BodyType::ShapeNode& sn)  // Used by Packet::Read<T>(T&)
        {
            p.Read(sn.name);
            p.Read(sn.children);
            p.Read(sn.parent);
            sn.shapeDesc = p.Read<Common::Physics::ShapeDesc>().release();
        }

        private:
        static std::unique_ptr<Common::Physics::BodyType::ShapeNode> Read(ByteArray const& p); // Used by Packet::Read<T>()
    };

}

#endif
