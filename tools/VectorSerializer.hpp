#ifndef __TOOLS_VECTORSERIALIZER_HPP__
#define __TOOLS_VECTORSERIALIZER_HPP__

#include "tools/ByteArray.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"

namespace Tools {

    template<typename T> struct ByteArray::Serializer< Tools::Vector3<T> >
    {
        static void Read(ByteArray const& p, Tools::Vector3<T>& v)  // Used by Packet::Read<T>(T&)
        {
            p.Read(v.x);
            p.Read(v.y);
            p.Read(v.z);
        }

        static void Write(Tools::Vector3<T> const& v, ByteArray& p) // Used by Packet::Write<T>(T const&)
        {
            p.Write(v.x);
            p.Write(v.y);
            p.Write(v.z);
        }

    private:
        static std::unique_ptr<Tools::Vector3<T>> Read(ByteArray const& p); // Used by Packet::Read<T>()
        // TODO
    };

}

#endif
