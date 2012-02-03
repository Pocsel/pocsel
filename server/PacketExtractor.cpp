#include "server/PacketExtractor.hpp"

#include "common/Packet.hpp"

namespace Server {

    void PacketExtractor::Login(Common::Packet const& p,
            Protocol::Version& major,
            Protocol::Version& minor,
            std::string& login)
    {
        p.Read(major);
        p.Read(minor);
        p.Read(login);
    }

    /**
     * String contenant yalap_a (optimisé)
     */
    void PacketExtractor::Pong(Common::Packet const& p,
            std::string& str)
    {
        p.Read(str);
    }

    /**
     * (Uint64 chunkId)+
     */
    void PacketExtractor::NeedChunks(Common::Packet const& p,
            std::list<Chunk::IdType>& Ids)
    {
        while (p.GetBytesLeft())
        {
            static_assert(sizeof(Chunk::IdType) == 8, "faut changer le read64");
            Ids.push_back(p.Read64());
        }
    }

    /**
     * Uint32 page
     */
    void PacketExtractor::ExtractGetCubeType(Common::Packet const& p, Chunk::CubeType& id)
    {
        p.Read(id);
    }

    void PacketExtractor::ExtractGetResourcesIds(Common::Packet const& p, Uint32& page)
    {
        p.Read(page);
    }

    void PacketExtractor::ExtractGetSpawnPosition(Common::Packet const&)
    {
    }

}
