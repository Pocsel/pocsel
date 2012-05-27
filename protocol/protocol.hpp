#ifndef __PROTOCOL_PROTOCOL_HPP__
#define __PROTOCOL_PROTOCOL_HPP__

#include "tools/array_size.hpp"
#include "tools/enum.hpp"
#include "tools/ByteArray.hpp"

namespace Protocol {

    ENUM Version
    {
        Major = 0,
        Minor = 1
    };

    ENUM ClientToServer
    {
        /**
         * (network specific)
         * bool Ok
         */
        UdpReady = 0,

        /**
         * (network specific)
         * void
         */
        ClPassThrough,

        /**
         * (network specific)
         * void
         */
        ClPassThroughOk,

        /**
         * Int32 Version Major
         * Int32 Version Minor
         * String login
         */
        Login,

        /**
         * Uint64 timestamp du packet ping
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
         * Uint32 view distance
         * String player name
         */
        Settings,

        /**
         * Rien
         */
        TeleportOk,

        /**
         * MovingOrientedPosition nouvelle position
         */
        Move,

        /**
         * OrientedPosition position
         * CubePosition target
         * Uint32 actionId
         */
        Action,

        /**
         * Uint32 doodadId
         */
        DoodadRemoved,

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
         *   Uint32 clientId
         *   String world identifier
         *   String world name
         *   Uint32 world version
         *   Uint16 (CubeType) nombre de types de cube
         *   String world build hash - ajouté le 28/2/12
         */
        LoggedIn = 0,

        /**
         * (network specific)
         * void
         */
        SvPassThrough,

        /**
         * (network specific)
         * void
         */
        SvPassThroughOk,

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
         * String Map
         * Position pos
         */
        TeleportPlayer,

        /**
         * MovingOrientedPosition pos
         * Uint32 item id
         */
        ItemMove,

        /**
         * Uint32 doodadId
         * Uint32 pluginId
         * String doodadName
         * Position position
         * ? String serializedData
         */
        DoodadSpawn,

        /**
         * Uint32 doodadId
         */
        DoodadKill,

        /**
         * Uint32 doodadId
         * bool position
         * ? Position p
         * ? String serializedData
         */
        DoodadUpdate,

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
                "UdpReady",
                "ClPassThrough",
                "ClPassThroughOk",
                "Login",
                "Pong",
                "NeedChunks",
                "GetNeededResourceIds",
                "GetResourceRange",
                "GetCubeType",
                "Settings",
                "TeleportOk",
                "Move",
                "Action"
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
                "SvPassThrough",
                "SvPassThroughOk",
                "Ping",
                "Chunk",
                "NeededResourceIds",
                "ResourceRange",
                "CubeType",
                "TeleportPlayer",
                "ItemMove"
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

}

#endif
