#ifndef __TST_PROTOCOL_TST_PROTOCOL_HPP__
#define __TST_PROTOCOL_TST_PROTOCOL_HPP__

#include "tools/array_size.hpp"
#include "tools/enum.hpp"
#include "tools/ByteArray.hpp"

namespace tst_protocol {

    ENUM Version
    {
        Major = 0,
        Minor = 1
    };

    ENUM ClientToServer
    {
        // Uint32 type
        clPassThrough,
        // Uint32 type
        clPassThroughOk,

        nbPacketTypeClient, // pour les tableaux
    };

    ENUM ServerToClient
    {
        // Uint32 id
        svLogin,
        // Uint32 type
        svPassThrough,
        // Uint32 type
        svPassThroughOk,

        nbPacketTypeServer, // pour les tableaux
    };

    typedef Uint8 ActionType;
}

namespace Tools {

    template<> struct ByteArray::Serializer<tst_protocol::ClientToServer>
    {
        static void Read(ByteArray const& b, tst_protocol::ClientToServer& v) // Used by ByteArray::Read<T>(T&)
        {
            b.Read((tst_protocol::ActionType&)v);
        }

        static void Write(tst_protocol::ClientToServer const& v, ByteArray& b) // Used by ByteArray::Write<T>(T const&)
        {
            b.Write((tst_protocol::ActionType)v);
        }
    };

    template<> struct ByteArray::Serializer<tst_protocol::ServerToClient>
    {
        static void Read(ByteArray const& b, tst_protocol::ServerToClient& v) // Used by ByteArray::Read<T>(T&)
        {
            b.Read((tst_protocol::ActionType&)v);
        }

        static void Write(tst_protocol::ServerToClient const& v, ByteArray& b) // Used by ByteArray::Write<T>(T const&)
        {
            b.Write((tst_protocol::ActionType)v);
        }
    };

    template<> struct ByteArray::Serializer<tst_protocol::Version>
    {
        static void Read(ByteArray const& b, tst_protocol::Version& v) // Used by ByteArray::Read<T>(T&)
        {
            v = (tst_protocol::Version) b.Read32();
        }

        static void Write(tst_protocol::Version const& v, ByteArray& b) // Used by ByteArray::Write<T>(T const&)
        {
            b.Write32((Int32)v);
        }
    };

}

#endif

