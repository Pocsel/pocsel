#ifndef __SERVER_NETWORK_PACKETCREATOR_HPP__
#define __SERVER_NETWORK_PACKETCREATOR_HPP__

#include "server/game/map/Chunk.hpp"

namespace Common{
    class Packet;
    class Resource;
    struct CubeType;
    struct Position;
    struct MovingOrientedPosition;
}

namespace Server { namespace Network {
    class UdpPacket;
}}

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
                                        std::string const& worldBuildHash = "");

        static std::unique_ptr<UdpPacket> PassThrough();

        static std::unique_ptr<Common::Packet> PassThroughOk();

        static std::unique_ptr<Common::Packet> Ping(Uint64 timestamp);

        static std::unique_ptr<Common::Packet> Chunk(::Server::Game::Map::Chunk const& chunk);

        static std::unique_ptr<Common::Packet> NeededResourceIds(std::vector<Uint32>& ids,
                                                 Uint32& offset);

        static std::unique_ptr<Common::Packet> ResourceRange(Common::Resource const& resource,
                                             Uint32 offset);

        static std::unique_ptr<Common::Packet> CubeType(Common::CubeType const& cubeType);

        static std::unique_ptr<Common::Packet> TeleportPlayer(std::string const& map,
                                                              Common::Position const& pos);

        static std::unique_ptr<UdpPacket> ItemMove(Common::MovingOrientedPosition const& pos,
                                                   Uint32 itemId);
    };

}}

#endif
