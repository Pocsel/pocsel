#ifndef __SERVER_PACKETEXTRACTOR_HPP__
#define __SERVER_PACKETEXTRACTOR_HPP__

#include <string>
#include <list>

#include "server/Chunk.hpp"
#include "protocol/protocol.hpp"

namespace Common {

    class Packet;

}

namespace Server {

    class PacketExtractor
    {
        public:
        /**
         * Int32 Version Major
         * Int32 Version Minor
         * String login
         */
        static void Login(Common::Packet const& p,
                          Protocol::Version& major,
                          Protocol::Version& minor,
                          std::string& login);

        /**
         * String contenant yalap_a (optimisé)
         */
        static void Pong(Common::Packet const& p,
                         std::string& str);

        /**
         * (Uint64 chunkId)+
         */
        static void NeedChunks(Common::Packet const& p,
                               std::list<Chunk::IdType>& Ids);

        static void ExtractGetCubeType(Common::Packet const&, Chunk::CubeType& id);
        static void ExtractGetResourcesIds(Common::Packet const&, Uint32& page);

        static void ExtractGetSpawnPosition(Common::Packet const& p);
    };

}

#endif
