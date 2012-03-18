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
        static void ExtractLogin(Tools::ByteArray const& p,
                bool& status,
                Protocol::Version& major,
                Protocol::Version& minor,
                std::string& reason,
                Uint32& clientId,
                std::string& worldIdentifier,
                std::string& worldName,
                Uint32& worldVersion,
                Common::BaseChunk::CubeType& nbCubeTypes,
                std::string& worldBuildHash);
        static Uint64 ExtractPing(Tools::ByteArray const& p);
        static void ExtractNeededResourceIds(Tools::ByteArray const& p, Uint32& nbResourceIds, std::list<Uint32>& ids);
        static char const* ExtractResourceRange(Tools::ByteArray const& p,
                Uint32& id,
                Uint32& offset,
                Uint32& pluginId,
                std::string& type,
                std::string& filename,
                Uint32& totalSize,
                Uint32& dataSize);
        static std::unique_ptr<Map::Chunk> ExtractChunk(Tools::ByteArray const& p);
        static std::unique_ptr<Common::CubeType> ExtractCubeType(Tools::ByteArray const& p);

        static void ExtractTeleportPlayer(Tools::ByteArray const& p, std::string& map, Common::Position& position);
    };

}}

#endif
