#ifndef __SERVER_NETWORK_PACKETCREATOR_HPP__
#define __SERVER_NETWORK_PACKETCREATOR_HPP__

#include "server2/Chunk.hpp"

namespace Common{

    class Packet;
    class Resource;
    struct CubeType;
    struct Position;

}

namespace Server { namespace Network {

    class PacketCreator
    {
    public:
        static std::unique_ptr<Common::Packet> LoggedIn(bool success,
                                        std::string const& reason = "",
                                        std::string const& worldIdentifier = "",
                                        std::string const& worldName = "",
                                        Uint32 worldVersion = 0,
                                        Chunk::CubeType nbCubeTypes = 0);

        static std::unique_ptr<Common::Packet> Ping(Uint64 timestamp);

        static std::unique_ptr<Common::Packet> Chunk(::Server::Chunk const& chunk);

        static std::unique_ptr<Common::Packet> NeededResourceIds(std::vector<Uint32>& ids,
                                                 Uint32& offset);

        static std::unique_ptr<Common::Packet> ResourceRange(Common::Resource const& resource,
                                             Uint32 offset);

        static std::unique_ptr<Common::Packet> CubeType(Common::CubeType const& cubeType);

        static std::unique_ptr<Common::Packet> TeleportPlayer(Common::Position const& pos);
    };

}}

#endif
