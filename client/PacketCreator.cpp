#include "client/PacketCreator.hpp"

#include "protocol/protocol.hpp"

#include "common/Packet.hpp"

namespace Client {

    Common::PacketPtr PacketCreator::LoginPacket(std::string const& login)
    {
        Common::PacketPtr p(new Common::Packet);
        p->Write(Protocol::ClientToServer::Login);

        p->Write(Protocol::Version::Major);
        p->Write(Protocol::Version::Minor);
        p->Write(login);
        return p;
    }

    Common::PacketPtr PacketCreator::PongPacket()
    {
        Common::PacketPtr p(new Common::Packet);
        p->Write(Protocol::ClientToServer::Pong);

        p->Write(std::string("yalap_a"));
        return p;
    }

    Common::PacketPtr PacketCreator::NeedChunksPacket(std::deque<Chunk::IdType>& chunkIds)
    {
        Common::PacketPtr p(new Common::Packet);
        p->Write(Protocol::ClientToServer::NeedChunks);

        for (; !chunkIds.empty() && (p->GetSize() + sizeof(Chunk::IdType)) < (1 << 16); chunkIds.pop_back())
            p->Write(chunkIds.back());
        return p;
    }

    Common::PacketPtr PacketCreator::GetNeededResourceIdsPacket(Uint32 cacheVersion)
    {
        Common::PacketPtr p(new Common::Packet);
        p->Write(Protocol::ClientToServer::GetNeededResourceIds);

        p->Write(cacheVersion);
        return p;
    }

    Common::PacketPtr PacketCreator::GetResourceRangePacket(Uint32 resourceId, Uint32 offset)
    {
        Common::PacketPtr p(new Common::Packet);
        p->Write(Protocol::ClientToServer::GetResourceRange);

        p->Write(resourceId);
        p->Write(offset);
        return p;
    }

    Common::PacketPtr PacketCreator::GetCubeTypePacket(Chunk::CubeType id)
    {
        Common::PacketPtr p(new Common::Packet);
        p->Write(Protocol::ClientToServer::GetCubeType);

        p->Write(id);
        return p;
    }

    Common::PacketPtr PacketCreator::GetSpawnPositionPacket()
    {
        Common::PacketPtr p(new Common::Packet);
        p->Write(Protocol::ClientToServer::GetSpawnPosition);
        return  p;
    }

}
