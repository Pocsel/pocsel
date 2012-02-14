#ifndef __CLIENT_NETWORK_PACKETCREATOR_HPP__
#define __CLIENT_NETWORK_PACKETCREATOR_HPP__

#include "common/BaseChunk.hpp"
#include "common/Packet.hpp"

namespace Client {
    class Settings;
}

namespace Client { namespace Network {

    class PacketCreator
    {
    public:
        static std::unique_ptr<Common::Packet> Login(std::string const& login);
        static std::unique_ptr<Common::Packet> Pong(Uint64 timestamp);
        //static std::unique_ptr<Common::Packet> NeedChunks(std::deque<Chunk::IdType>& chunkIds);
        static std::unique_ptr<Common::Packet> GetNeededResourceIds(Uint32 cacheVersion);
        static std::unique_ptr<Common::Packet> GetResourceRange(Uint32 resourceId, Uint32 offset);
        static std::unique_ptr<Common::Packet> GetCubeType(Common::BaseChunk::CubeType id);
        //static std::unique_ptr<Common::Packet> GetSpawnPosition();
        static std::unique_ptr<Common::Packet> Settings(Settings const& settings);
    };

}}

#endif
