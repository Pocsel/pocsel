#ifndef __PROTOCOL_HPP__
#define __PROTOCOL_HPP__

#include "tools/ToString.hpp"
#include "tools/array_size.hpp"
#include "tools/enum.hpp"
#include "tools/ByteArray.hpp"

namespace Protocol
{
    ENUM Version
    {
        Major = 0,
        Minor = 1
    };

    ENUM ClientToServer
    {
        /**
         * Int32 Version Major
         * Int32 Version Minor
         * String login
         */
        Login = 0,

        /**
         * String contenant yalap_a (optimisé)
         */
        Pong,

        /**
         * (Uint64 chunkId)+
         */
        NeedChunks,

        /**
         * Uint32 cache version
         */
        GetNeededResourceIds,

        /**
         * Uint32 resource id
         * Uint32 offset
         */
        GetResourceRange,

        /**
         * Uint16 (CubeType) id
         */
        GetCubeType,

        /**
         * void
         */
        GetSpawnPosition,

        NbPacketTypeClient, // pour les tableaux
    };

    ENUM ServerToClient
    {
        /**
         * Bool status
         * Int32 Version Major
         * Int32 Version Minor
         *  Si status == false :
         *   String reason
         *  Sinon :
         *   String world identifier
         *   String world name
         *   Uint32 world version
         *   Uint16 (CubeType) nombre de types de cube
         */
        LoggedIn = 0,

        /**
         * Uint64 timestamp
         */
        Ping,

        /**
         * SerializedChunk
         */
        Chunk,

        /**
         * Uint32 nombre d'id au total (peut etre plus grand que le nombre d'id dans ce paquet)
         * Uint32+ resource ids
         */
        NeededResourceIds,

        /**
         * Uint32 id
         * Uint32 offset
         *  Si offset == 0:
         *      Uint32 pluginId
         *      String type
         *      String filename
         *      Uint32 size
         *  Char+ raw data de la taille restante
         */
        ResourceRange,

        /**
         * CubeType description
         */
        CubeType,

        /**
         * Position pos
         */
        SpawnPosition,

        NbPacketTypeServer, // pour les tableaux
    };

    typedef Uint8 ActionType;
}

namespace Tools {

template<> struct ByteArray::Serializer<Protocol::ClientToServer>
{
    static void Read(ByteArray const& b, Protocol::ClientToServer& v) // Used by ByteArray::Read<T>(T&)
    {
        b.Read((Protocol::ActionType&)v);
    }

    static void Write(Protocol::ClientToServer const& v, ByteArray& b) // Used by ByteArray::Write<T>(T const&)
    {
        b.Write((Protocol::ActionType)v);
    }
};

template<> struct ByteArray::Serializer<Protocol::ServerToClient>
{
    static void Read(ByteArray const& b, Protocol::ServerToClient& v) // Used by ByteArray::Read<T>(T&)
    {
        b.Read((Protocol::ActionType&)v);
    }

    static void Write(Protocol::ServerToClient const& v, ByteArray& b) // Used by ByteArray::Write<T>(T const&)
    {
        b.Write((Protocol::ActionType)v);
    }
};

template<> struct ByteArray::Serializer<Protocol::Version>
{
    static void Read(ByteArray const& b, Protocol::Version& v) // Used by ByteArray::Read<T>(T&)
    {
        v = (Protocol::Version) b.Read32();
    }

    static void Write(Protocol::Version const& v, ByteArray& b) // Used by ByteArray::Write<T>(T const&)
    {
        b.Write32((Int32)v);
    }
};

template<> struct Stringify<Protocol::ClientToServer>
{
    static inline std::string MakeString(Protocol::ClientToServer var)
    {
        static std::string strings[] = {
            "Login",
            "Pong",
            "NeedChunks",
            "GetNeededResourceIds",
            "GetResourceRange",
            "GetCubeType",
            "GetSpawnPosition"
        };
        static std::string unknown = "Unknown";

        if (((Protocol::ActionType) var) < array_size(strings))
            return strings[(Protocol::ActionType) var];
        else
            return unknown;
    }
};

template<> struct Stringify<Protocol::ServerToClient>
{
    static inline std::string MakeString(Protocol::ServerToClient var)
    {
        static std::string strings[] = {
            "LoggedIn",
            "Ping",
            "Chunk",
            "NeededResourceIds",
            "ResourceRange",
            "CubeType",
            "SpawnPosition"
        };
        static std::string unknown = "Unknown";

        if ((Protocol::ActionType) var < array_size(strings))
            return strings[(Protocol::ActionType) var];
        else
            return unknown;
    }
};

template<> struct Stringify<Protocol::Version>
{
    static inline std::string MakeString(Protocol::Version var)
    {
        return ToString((int) var);
    }
};

} // !Tools

#endif /* !__PROTOCOL_HPP__ */
