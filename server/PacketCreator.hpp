#ifndef __SERVER_PACKETCREATOR_HPP__
#define __SERVER_PACKETCREATOR_HPP__

#include <string>

#include "server/Chunk.hpp"
#include "common/PacketPtr.hpp"

namespace Common{
    class Resource;
    struct CubeType;
    struct Position;
}

namespace Server {

    class PacketCreator
    {
    public:
        static Common::PacketPtr LoggedInPacket(bool success,
                std::string const& reason = "",
                std::string const& worldIdentifier = "",
                std::string const& worldName = "",
                Uint32 worldVersion = 0,
                Chunk::CubeType nbCubeTypes = 0);
        static Common::PacketPtr PingPacket();
        static Common::PacketPtr ChunkPacket(Chunk const& chunk);
        static Common::PacketPtr ResourceRangePacket(Common::Resource const& resource, Uint32 offset);
        static Common::PacketPtr CubeTypePacket(Common::CubeType const& cubeType);
        static Common::PacketPtr NeededResourceIdsPacket(std::vector<Uint32>& ids, Uint32& offset);
        static Common::PacketPtr TeleportPlayer(Common::Position const& pos);
    };

}

#endif
