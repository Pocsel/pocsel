#ifndef __CLIENT_NETWORK_PACKETEXTRACTOR_HPP__
#define __CLIENT_NETWORK_PACKETEXTRACTOR_HPP__

#include <string>
#include <list>

#include "protocol/protocol.hpp"
#include "client/map/Chunk.hpp"
#include "common/CubeType.hpp"

namespace Common {
    class Packet;
    struct Position;
}

namespace Client { namespace Network {

    class PacketExtractor
    {
    public:
        static void ExtractLogin(Common::Packet const& p,
                bool& status,
                Protocol::Version& major,
                Protocol::Version& minor,
                std::string& reason,
                std::string& worldIdentifier,
                std::string& worldName,
                Uint32& worldVersion,
                Common::BaseChunk::CubeType& nbCubeTypes);
        static Uint64 ExtractPing(Common::Packet const& p);
        static void ExtractNeededResourceIds(Common::Packet const& p, Uint32& nbResourceIds, std::list<Uint32>& ids);
        static char const* ExtractResourceRange(Common::Packet const& p,
                Uint32& id,
                Uint32& offset,
                Uint32& pluginId,
                std::string& type,
                std::string& filename,
                Uint32& totalSize,
                Uint32& dataSize);
        static std::unique_ptr<Map::Chunk> ExtractChunk(Common::Packet const& p);
        static std::unique_ptr<Common::CubeType> ExtractCubeType(Common::Packet const& p);

        static void ExtractTeleportPlayer(Common::Packet const& p, std::string& map, Common::Position& position);
    };

}}

#endif
