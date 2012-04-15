#ifndef __TOOLS_VECTORSERIALIZER_HPP__
#define __TOOLS_VECTORSERIALIZER_HPP__

#include "tools/ByteArray.hpp"
#include "tools/Vector2.hpp"
#include "tools/Vector3.hpp"

namespace Tools {

    // Packet
    template<typename T> struct ByteArray::Serializer< glm::detail::tvec2<T> >
    {
        static void Read(ByteArray const& p, glm::detail::tvec2<T>& v)  // Used by Packet::Read<T>(T&)
        {
            p.Read(v.x);
            p.Read(v.y);
        }

        static void Write(glm::detail::tvec2<T> const& v, ByteArray& p) // Used by Packet::Write<T>(T const&)
        {
            p.Write(v.x);
            p.Write(v.y);
        }

    private:
        static std::unique_ptr<glm::detail::tvec2<T>> Read(ByteArray const& p); // Used by Packet::Read<T>()
        // TODO
    };

    template<typename T> struct ByteArray::Serializer< glm::detail::tvec3<T> >
    {
        static void Read(ByteArray const& p, glm::detail::tvec3<T>& v)  // Used by Packet::Read<T>(T&)
        {
            p.Read(v.x);
            p.Read(v.y);
            p.Read(v.z);
        }

        static void Write(glm::detail::tvec3<T> const& v, ByteArray& p) // Used by Packet::Write<T>(T const&)
        {
            p.Write(v.x);
            p.Write(v.y);
            p.Write(v.z);
        }

    private:
        static std::unique_ptr<glm::detail::tvec3<T>> Read(ByteArray const& p); // Used by Packet::Read<T>()
        // TODO
    };

}

#endif
