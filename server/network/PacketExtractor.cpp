#include "server2/network/PacketExtractor.hpp"

#include "common/Packet.hpp"

namespace Server { namespace Network {

    void PacketExtractor::Login(Common::Packet const& p,
                                Protocol::Version& major,
                                Protocol::Version& minor,
                                std::string& login)
    {
        p.Read(major);
        p.Read(minor);
        p.Read(login);
    }

    void PacketExtractor::Pong(Common::Packet const& p,
                               Uint64& timestamp)
    {
        p.Read(timestamp);
    }

    void PacketExtractor::NeedChunks(Common::Packet const& p,
                                     std::vector<Chunk::IdType>& ids)
    {
        unsigned int size = p.GetBytesLeft() / sizeof(Chunk::IdType);

        ids.resize(size);

        Chunk::IdType* id = &ids[0];

        while (size)
        {
            p.Read(*id);
            ++id;
            --size;
        }
    }

    void PacketExtractor::GetNeededResourceIds(Common::Packet const& p, Uint32& version)
    {
        p.Read(version);
    }

    void PacketExtractor::GetResourceRange(Common::Packet const& p,
                                           Uint32& resourceId,
                                           Uint32& offset)
    {
        p.Read(resourceId);
        p.Read(offset);
    }

    void PacketExtractor::GetCubeType(Common::Packet const& p, Chunk::CubeType& id)
    {
        p.Read(id);
    }

    void PacketExtractor::Settings(Common::Packet const& p,
                                   Uint32& viewDistance,
                                   std::string& playerName)
    {
        p.Read(viewDistance);
        p.Read(playerName);
    }

    void PacketExtractor::TeleportOk(Common::Packet const&)
    {
    }

}}
