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
            std::string& str)
    {
        p.Read(str);
    }

    void PacketExtractor::NeedChunks(Common::Packet const& p,
            std::list<Chunk::IdType>& ids)
    {
        while (p.GetBytesLeft())
        {
            static_assert(sizeof(Chunk::IdType) == 8, "faut changer le read64");
            ids.push_back(p.Read64());
        }
    }

    void PacketExtractor::GetNeededResourceIds(Common::Packet const& p, Uint32& page)
    {
        p.Read(page);
    }

    void PacketExtractor::GetCubeType(Common::Packet const& p, Chunk::CubeType& id)
    {
        p.Read(id);
    }

    void PacketExtractor::GetSpawnPosition(Common::Packet const&)
    {
    }

}}
