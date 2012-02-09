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
                         std::string& str);

        static void NeedChunks(Common::Packet const& p,
                               std::list<Chunk::IdType>& Ids);

        static void GetNeededResourceIds(Common::Packet const&,
                                         Uint32& page);

        static void GetResourceRange(Common::Packet const&,
                                     Uint32& resourceId,
                                     Uint32& offset);

        static void GetCubeType(Common::Packet const&,
                                Chunk::CubeType& id);

        static void GetSpawnPosition(Common::Packet const& p);
    };

}}

#endif
