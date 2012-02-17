#include "client2/precompiled.hpp"

#include "client2/network/ChunkSerializer.hpp"
#include "client2/network/PacketExtractor.hpp"
#include "common/Packet.hpp"
#include "common/CubeTypeSerializer.hpp"
#include "common/PositionSerializer.hpp"

namespace Client { namespace Network {

    void PacketExtractor::ExtractLogin(Common::Packet const& p,
            bool& status,
            Protocol::Version& major,
            Protocol::Version& minor,
            std::string& reason,
            std::string& worldIdentifier,
            std::string& worldName,
            Uint32& worldVersion,
            Common::BaseChunk::CubeType& nbCubeTypes)
    {
        p.Read(status);
        p.Read(major);
        p.Read(minor);
        if (status == false)
            p.Read(reason);
        else
        {
            p.Read(worldIdentifier);
            p.Read(worldName);
            p.Read(worldVersion);
            p.Read(nbCubeTypes);
            if (nbCubeTypes == 0)
                throw std::runtime_error("nbCubeTypes == 0");
        }
    }

    Uint64 PacketExtractor::ExtractPing(Common::Packet const& p)
    {
        return p.Read64();
    }

    void PacketExtractor::ExtractNeededResourceIds(Common::Packet const& p, Uint32& nbResourceIds, std::list<Uint32>& ids)
    {
        p.Read(nbResourceIds);
        unsigned int nbIds = p.GetBytesLeft() / sizeof(Uint32);
        for (unsigned int i = 0; i < nbIds; ++i)
            ids.push_back(p.Read32());
    }

    char const* PacketExtractor::ExtractResourceRange(Common::Packet const& p,
                                                      Uint32& id,
                                                      Uint32& offset,
                                                      Uint32& pluginId,
                                                      std::string& type,
                                                      std::string& filename,
                                                      Uint32& totalSize,
                                                      Uint32& dataSize)
    {
        p.Read(id);
        p.Read(offset);
        if (!offset)
        {
            p.Read(pluginId);
            p.Read(type);
            p.Read(filename);
            p.Read(totalSize);
        }
        dataSize = p.GetBytesLeft();
        return p.ReadRawData(dataSize);
    }

    std::unique_ptr<Map::Chunk> PacketExtractor::ExtractChunk(Common::Packet const& p)
    {
        return p.Read<Map::Chunk>();
    }

    std::unique_ptr<Common::CubeType> PacketExtractor::ExtractCubeType(Common::Packet const& p)
    {
        return p.Read<Common::CubeType>();
    }

    void PacketExtractor::ExtractTeleportPlayer(Common::Packet const& p, std::string& map, Common::Position& position)
    {
        p.Read(map);
        p.Read(position);
    }

}}
