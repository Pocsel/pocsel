#ifndef __SERVER_NETWORK_PACKETCREATOR_HPP__
#define __SERVER_NETWORK_PACKETCREATOR_HPP__

#include "server/game/map/Chunk.hpp"
#include "common/Position.hpp"

namespace Common {
    class Packet;
    class Resource;
    struct CubeType;
    struct MovingOrientedPosition;
    namespace Physics {
        struct Node;
        struct Body;
    }
}

namespace Server {
    namespace Game { namespace Engine {
        class BodyType;
    }}
    namespace Game {
        class PluginManager;
    }
    namespace Network {
        class UdpPacket;
    }
}

namespace Server { namespace Network {

    class PacketCreator
    {
    public:
        static std::unique_ptr<Common::Packet> LoggedIn(bool success,
                                        std::string const& reason = "",
                                        Uint32 clientId = 0,
                                        std::string const& worldIdentifier = "",
                                        std::string const& worldName = "",
                                        Uint32 worldVersion = 0,
                                        Game::Map::Chunk::CubeType nbCubeTypes = 0,
                                        Uint32 nbBodyTypes = 0,
                                        std::string const& worldBuildHash = "");

        static std::unique_ptr<UdpPacket> PassThrough();
        static std::unique_ptr<Common::Packet> PassThroughOk();

        static std::unique_ptr<Common::Packet> Ping(Uint64 timestamp);

        static std::unique_ptr<Common::Packet> Chunk(::Server::Game::Map::Chunk const& chunk);

        static std::unique_ptr<Common::Packet> NeededResourceIds(std::vector<Uint32>& ids,
                                                 Uint32& offset);

        static std::unique_ptr<Common::Packet> ResourceRange(Game::PluginManager const& pluginManager,
            Common::Resource const& resource,
            Uint32 offset);

        static std::unique_ptr<Common::Packet> CubeType(Common::CubeType const& cubeType);

        static std::unique_ptr<Common::Packet> BodyType(Game::Engine::BodyType const& bodyType);

        static std::unique_ptr<Common::Packet> TeleportPlayer(std::string const& map,
                                                              Common::Position const& pos);

        static std::unique_ptr<Common::Packet> DoodadSpawn(Uint32 doodadId,
                Uint32 entityId,
                std::string const& doodadName,
                Common::Physics::Node const& position,
                Uint32 bodyId,
                std::list<std::pair<std::string /* key */, std::string /* value */>> const& values);

        static std::unique_ptr<Common::Packet> DoodadKill(Uint32 doodadId);

        static std::unique_ptr<UdpPacket> DoodadUpdate(Uint32 doodadId,
                Common::Physics::Body const* body,
                std::list<std::tuple<bool /* functionCall */, std::string /* function */, std::string /* value */>> const& commands);

        static std::unique_ptr<UdpPacket> EntityUpdate(Uint32 entityId, Common::Physics::Node const& position);
    };

}}

#endif
