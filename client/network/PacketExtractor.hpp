#ifndef __CLIENT_NETWORK_PACKETEXTRACTOR_HPP__
#define __CLIENT_NETWORK_PACKETEXTRACTOR_HPP__

#include <string>
#include <list>

#include "protocol/protocol.hpp"
#include "client/map/Chunk.hpp"
#include "common/CubeType.hpp"
#include "common/Position.hpp"

namespace Common {
    class Packet;
    struct MovingOrientedPosition;
    namespace Physics {
        struct Node;
        class Body;
    }
}

namespace Client { namespace Game { namespace Engine {
    class BodyType;
}}}

namespace Client { namespace Network {

    class PacketExtractor
    {
    public:
        static void Login(Tools::ByteArray const& p,
                bool& status,
                Protocol::Version& major,
                Protocol::Version& minor,
                std::string& reason,
                Uint32& clientId,
                std::string& worldIdentifier,
                std::string& worldName,
                Uint32& worldVersion,
                Common::BaseChunk::CubeType& nbCubeTypes,
                Uint32& nbBodyTypes,
                std::string& worldBuildHash);
        static Uint64 Ping(Tools::ByteArray const& p);
        static void NeededResourceIds(Tools::ByteArray const& p, Uint32& nbResourceIds, std::list<Uint32>& ids);
        static char const* ResourceRange(Tools::ByteArray const& p,
                Uint32& id,
                Uint32& offset,
                std::string& type,
                std::string& filename,
                Uint32& totalSize,
                Uint32& dataSize);
        static std::unique_ptr<Map::Chunk> Chunk(Tools::ByteArray const& p);
        static std::unique_ptr<Common::CubeType> CubeType(Tools::ByteArray const& p);
        static std::unique_ptr<Game::Engine::BodyType> BodyType(Tools::ByteArray const& p);

        static void TeleportPlayer(Tools::ByteArray const& p, std::string& map, Common::Position& position);

        static void DoodadSpawn(Tools::ByteArray const& p,
                Uint32& doodadId,
                Uint32& entityId,
                std::string& doodadName,
                Common::Physics::Node& position,
                Uint32& bodyId,
                std::list<std::pair<std::string /* key */, std::string /* value */>>& values);
        static void DoodadKill(Tools::ByteArray const& p,
                Uint32& doodadId);
        static void DoodadUpdate(Tools::ByteArray const& p,
                Uint32& doodadId,
                std::unique_ptr<std::vector<std::pair<bool, Common::Physics::Node>>>& body,
                std::list<std::tuple<bool /* functionCall */, std::string /* function || key */, std::string /* value */>>& commands);
        static void EntityUpdate(Tools::ByteArray const& p,
                Uint32& entityId,
                std::vector<Common::Physics::Node>& node);
    };

}}

#endif
