#ifndef __SERVER_NETWORK_PACKETCREATOR_HPP__
#define __SERVER_NETWORK_PACKETCREATOR_HPP__

#include <string>

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
        static Common::Packet* LoggedIn(bool success,
                                        std::string const& reason = "",
                                        std::string const& worldIdentifier = "",
                                        std::string const& worldName = "",
                                        Uint32 worldVersion = 0,
                                        Chunk::CubeType nbCubeTypes = 0);

        static Common::Packet* Ping();

        static Common::Packet* Chunk(::Server::Chunk const& chunk);

        static Common::Packet* NeededResourceIds(std::vector<Uint32>& ids,
                                                 Uint32& offset);

        static Common::Packet* ResourceRange(Common::Resource const& resource,
                                             Uint32 offset);

        static Common::Packet* CubeType(Common::CubeType const& cubeType);

        static Common::Packet* SpawnPosition(Common::Position const& pos);
    };

}}

#endif
