#include "server/network/PacketExtractor.hpp"

#include "common/Packet.hpp"
#include "common/OrientedPositionSerializer.hpp"
#include "common/MovingOrientedPositionSerializer.hpp"
#include "common/CubePositionSerializer.hpp"

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
                                     std::vector<Game::Map::Chunk::IdType>& ids)
    {
        unsigned int size = p.GetBytesLeft() / sizeof(Game::Map::Chunk::IdType);

        ids.resize(size);

        Game::Map::Chunk::IdType* id = &ids[0];

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

    void PacketExtractor::GetCubeType(Common::Packet const& p, Game::Map::Chunk::CubeType& id)
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

    void PacketExtractor::Move(Common::Packet const& p, Common::MovingOrientedPosition& pos)
    {
        p.Read(pos);
    }

    void PacketExtractor::Action(Common::Packet const& p, Common::OrientedPosition& pos, Common::CubePosition& cubePos, Uint32& actionId)
    {
        p.Read(pos);
        p.Read(cubePos);
        p.Read(actionId);
    }

}}
