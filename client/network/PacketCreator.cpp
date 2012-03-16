#include "client/Settings.hpp"
#include "client/network/PacketCreator.hpp"
#include "client/network/UdpPacket.hpp"

#include "protocol/protocol.hpp"

#include "common/Packet.hpp"
#include "common/CameraSerializer.hpp"
#include "common/CubePositionSerializer.hpp"

namespace Client { namespace Network {

    std::unique_ptr<Common::Packet> PacketCreator::Login(std::string const& login)
    {
        std::unique_ptr<Common::Packet> p(new Common::Packet());
        p->Write(Protocol::ClientToServer::Login);

        p->Write(Protocol::Version::Major);
        p->Write(Protocol::Version::Minor);
        p->Write(login);
        return p;
    }

    std::unique_ptr<Common::Packet> PacketCreator::Pong(Uint64 timestamp)
    {
        std::unique_ptr<Common::Packet> p(new Common::Packet());
        p->Write(Protocol::ClientToServer::Pong);

        p->Write(timestamp);
        return p;
    }

    std::unique_ptr<Common::Packet> PacketCreator::NeedChunks(std::deque<Common::BaseChunk::IdType>& chunkIds)
    {
        std::unique_ptr<Common::Packet> p(new Common::Packet());
        p->Write(Protocol::ClientToServer::NeedChunks);

        for (; !chunkIds.empty() && (p->GetSize() + sizeof(Common::BaseChunk::IdType)) < (1 << 16); chunkIds.pop_back())
            p->Write(chunkIds.back());
        return p;
    }

    std::unique_ptr<Common::Packet> PacketCreator::GetNeededResourceIds(Uint32 cacheVersion)
    {
        std::unique_ptr<Common::Packet> p(new Common::Packet());
        p->Write(Protocol::ClientToServer::GetNeededResourceIds);

        p->Write(cacheVersion);
        return p;
    }

    std::unique_ptr<Common::Packet> PacketCreator::GetResourceRange(Uint32 resourceId, Uint32 offset)
    {
        std::unique_ptr<Common::Packet> p(new Common::Packet());
        p->Write(Protocol::ClientToServer::GetResourceRange);

        p->Write(resourceId);
        p->Write(offset);
        return p;
    }

    std::unique_ptr<Common::Packet> PacketCreator::GetCubeType(Common::BaseChunk::CubeType id)
    {
        std::unique_ptr<Common::Packet> p(new Common::Packet());
        p->Write(Protocol::ClientToServer::GetCubeType);

        p->Write(id);
        return p;
    }

    std::unique_ptr<Common::Packet> PacketCreator::Settings(Client::Settings const& settings)
    {
        std::unique_ptr<Common::Packet> p(new Common::Packet());
        p->Write(Protocol::ClientToServer::Settings);

        p->Write(settings.chunkViewDistance + settings.chunkCacheArea);
        p->Write(settings.nickname);
        return p;
    }

    std::unique_ptr<Common::Packet> PacketCreator::TeleportOk()
    {
        std::unique_ptr<Common::Packet> p(new Common::Packet());
        p->Write(Protocol::ClientToServer::TeleportOk);
        return p;
    }


    std::unique_ptr<UdpPacket> Move(Uint32 id,
                                    Common::Camera const& cam,
                                    Tools::Vector3f const& movement)
    {
        std::unique_ptr<UdpPacket> p(new UdpPacket(id));
        p->Write(Protocol::ClientToServer::Move);

        p->Write(cam);
        p->Write(movement);
        return p;
    }

    std::unique_ptr<UdpPacket> PacketCreator::Action(Uint32 id,
                                                     Common::Camera const& cam,
                                                     Common::CubePosition const& target,
                                                     Uint32 actionId)
    {
        std::unique_ptr<UdpPacket> p(new UdpPacket(id));
        p->Write(Protocol::ClientToServer::Action);

        p->Write(cam);
        p->Write(target);
        p->Write(actionId);
        return p;
    }

}}
