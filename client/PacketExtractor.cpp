#include <iostream>
#include <stdexcept>

#include "client/PacketExtractor.hpp"
#include "common/Packet.hpp"
#include "client/ChunkSerializer.hpp"
#include "common/CubeTypeSerializer.hpp"
#include "common/PositionSerializer.hpp"

namespace Client {

    void PacketExtractor::ExtractLogin(Common::Packet const& p,
            bool& status,
            Protocol::Version& major,
            Protocol::Version& minor,
            std::string& reason,
            std::string& worldIdentifier,
            std::string& worldName,
            Uint32& worldVersion,
            Chunk::CubeType& nbCubeTypes)
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

    void PacketExtractor::ExtractPing(Common::Packet const& p,
            Uint64& timestamp)
    {
        p.Read(timestamp);
    }

    void PacketExtractor::ExtractNeededResourceIds(Common::Packet const& p,
            Uint32& nbResourceIds,
            std::list<Uint32>& ids)
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

    std::unique_ptr<Chunk> PacketExtractor::ExtractChunk(Common::Packet const& p)
    {
        return p.Read<Chunk>();
    }

    std::unique_ptr<Common::CubeType> PacketExtractor::ExtractCubeType(Common::Packet const& p)
    {
        return p.Read<Common::CubeType>();
    }

    void PacketExtractor::ExtractSpawnPosition(Common::Packet const& p, Common::Position& pos)
    {
        p.Read(pos);
    }
}
