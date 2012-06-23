#ifndef __TOOLS_QUATERNIONSERIALIZER_HPP__
#define __TOOLS_QUATERNIONSERIALIZER_HPP__

#include "tools/ByteArray.hpp"

namespace Tools {

    // Packet
    template<typename T> struct ByteArray::Serializer< glm::detail::tquat<T> >
    {
        static void Read(ByteArray const& p, glm::detail::tquat<T>& v)  // Used by Packet::Read<T>(T&)
        {
            p.Read(v.x);
            p.Read(v.y);
            p.Read(v.z);
            p.Read(v.w);
        }

        static void Write(glm::detail::tquat<T> const& v, ByteArray& p) // Used by Packet::Write<T>(T const&)
        {
            p.Write(v.x);
            p.Write(v.y);
            p.Write(v.z);
            p.Write(v.w);
        }

        static std::unique_ptr<glm::detail::tquat<T>> Read(ByteArray const& p) // Used by Packet::Read<T>()
        {
            glm::detail::tquat<T>* v = new glm::detail::tquat<T>();
            p.Read(v->x);
            p.Read(v->y);
            p.Read(v->z);
            p.Read(v->w);
            return std::unique_ptr<glm::detail::tquat<T>>(v);
        }
    };

}

#endif

