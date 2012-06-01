#ifndef __TOOLS_STDVECTORSERIALIZER_HPP__
#define __TOOLS_STDVECTORSERIALIZER_HPP__

#include "tools/ByteArray.hpp"

namespace Tools {

    template<typename T> struct ByteArray::Serializer< std::vector<T> >
    {
        static void Read(ByteArray const& p, std::vector<T>& v)  // Used by Packet::Read<T>(T&)
        {
            Uint32 size = p.Read32();
            v.resize(size);
            for (auto it = v.begin(), ite = v.end(); it != ite; ++it)
                p.Read(*it);
        }

        static void Write(std::vector<T> const& v, ByteArray& p) // Used by Packet::Write<T>(T const&)
        {
            p.Write32((Uint32)v.size());
            for (auto it = v.begin(), ite = v.end(); it != ite; ++it)
                p.Write(*it);
        }

    private:
        static std::unique_ptr<std::vector<T>> Read(ByteArray const& p); // Used by Packet::Read<T>()
    };

}

#endif
