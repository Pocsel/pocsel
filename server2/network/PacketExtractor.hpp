#ifndef __SERVER_NETWORK_PACKETEXTRACTOR_HPP__
#define __SERVER_NETWORK_PACKETEXTRACTOR_HPP__

#include "server2/Chunk.hpp"
#include "protocol/protocol.hpp"

namespace Common {

    class Packet;

}

namespace Server { namespace Network {

    class PacketExtractor
    {
    public:
        static void Login(Common::Packet const& p,
                          Protocol::Version& major,
                          Protocol::Version& minor,
                          std::string& login);

        static void Pong(Common::Packet const& p,
                         Uint64& timestamp);

        static void NeedChunks(Common::Packet const& p,
                               std::vector<Chunk::IdType>& ids);

        static void GetNeededResourceIds(Common::Packet const& p,
                                         Uint32& version);

        static void GetResourceRange(Common::Packet const& p,
                                     Uint32& resourceId,
                                     Uint32& offset);

        static void GetCubeType(Common::Packet const& p,
                                Chunk::CubeType& id);

        static void Settings(Common::Packet const& p,
                             Uint32& viewDistance,
                             std::string& playerName);

        static void TeleportOk(Common::Packet const& p);
    };

}}

#endif
