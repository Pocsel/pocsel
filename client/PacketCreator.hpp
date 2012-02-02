#ifndef __CLIENT_PACKETCREATOR_HPP__
#define __CLIENT_PACKETCREATOR_HPP__

#include <string>
#include <deque>

#include "common/PacketPtr.hpp"
#include "client/Chunk.hpp"

namespace Client {

    class PacketCreator
    {
    public:
        static Common::PacketPtr LoginPacket(std::string const& login);
        static Common::PacketPtr PongPacket();
        static Common::PacketPtr NeedChunksPacket(std::deque<Chunk::IdType>& chunkIds);
        static Common::PacketPtr GetNeededResourceIdsPacket(Uint32 cacheVersion);
        static Common::PacketPtr GetResourceRangePacket(Uint32 resourceId, Uint32 offset);
        static Common::PacketPtr GetCubeTypePacket(Chunk::CubeType id);
        static Common::PacketPtr GetSpawnPositionPacket();
    };

}

#endif
