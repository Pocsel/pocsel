#ifndef __CLIENT_NETWORK_PACKETCREATOR_HPP__
#define __CLIENT_NETWORK_PACKETCREATOR_HPP__

#include "common/BaseChunk.hpp"
#include "common/Packet.hpp"
#include "client/network/UdpPacket.hpp"
#include "common/CubePosition.hpp"

namespace Common {
    struct OrientedPosition;
    struct MovingOrientedPosition;
}

namespace Client {
    class Settings;
}

namespace Client { namespace Network {

    class PacketCreator
    {
    public:
        static std::unique_ptr<Common::Packet> UdpReady(bool ready);
        static std::unique_ptr<UdpPacket> PassThrough(Uint32 id);
        static std::unique_ptr<Common::Packet> PassThroughOk();
        static std::unique_ptr<Common::Packet> Login(std::string const& login);
        static std::unique_ptr<Common::Packet> Pong(Uint64 timestamp);
        static std::unique_ptr<Common::Packet> NeedChunks(std::deque<Common::BaseChunk::IdType>& chunkIds);
        static std::unique_ptr<Common::Packet> GetNeededResourceIds(Uint32 cacheVersion);
        static std::unique_ptr<Common::Packet> GetResourceRange(Uint32 resourceId, Uint32 offset);
        static std::unique_ptr<Common::Packet> GetCubeType(Common::BaseChunk::CubeType id);
        static std::unique_ptr<Common::Packet> GetBodyType(Uint32 id);
        static std::unique_ptr<Common::Packet> Settings(Settings const& settings);
        static std::unique_ptr<Common::Packet> TeleportOk();

        static std::unique_ptr<UdpPacket> Move(Uint32 id,
                                               Common::MovingOrientedPosition const& pos);
        static std::unique_ptr<UdpPacket> Action(Uint32 id,
                                                 Common::OrientedPosition const& pos,
                                                 Common::CubePosition const& target,
                                                 Uint32 actionId);

        static std::unique_ptr<UdpPacket> EntityRemoved(Uint32 id, Uint32 entityId);
    };

}}

#endif
