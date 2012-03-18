#ifndef __SERVER_NETWORK_PACKETEXTRACTOR_HPP__
#define __SERVER_NETWORK_PACKETEXTRACTOR_HPP__

#include "server/game/map/Chunk.hpp"
#include "protocol/protocol.hpp"

namespace Common {
    struct OrientedPosition;
    struct MovingOrientedPosition;
    struct CubePosition;
}

namespace Tools {
    class ByteArray;
}

namespace Server { namespace Network {

    class PacketExtractor
    {
    public:
        static void Login(Tools::ByteArray const& p,
                          Protocol::Version& major,
                          Protocol::Version& minor,
                          std::string& login);

        static void Pong(Tools::ByteArray const& p,
                         Uint64& timestamp);

        static void NeedChunks(Tools::ByteArray const& p,
                               std::vector<Game::Map::Chunk::IdType>& ids);

        static void GetNeededResourceIds(Tools::ByteArray const& p,
                                         Uint32& version);

        static void GetResourceRange(Tools::ByteArray const& p,
                                     Uint32& resourceId,
                                     Uint32& offset);

        static void GetCubeType(Tools::ByteArray const& p,
                                Game::Map::Chunk::CubeType& id);

        static void Settings(Tools::ByteArray const& p,
                             Uint32& viewDistance,
                             std::string& playerName);

        static void TeleportOk(Tools::ByteArray const& p);

        static void Move(Tools::ByteArray const& p, Common::MovingOrientedPosition& pos);
        static void Action(Tools::ByteArray const& p, Common::OrientedPosition& cam, Common::CubePosition& cubePos, Uint32& actionId);
    };

}}

#endif
