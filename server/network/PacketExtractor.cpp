#include "server/network/PacketExtractor.hpp"

#include "common/OrientedPositionSerializer.hpp"
#include "common/MovingOrientedPositionSerializer.hpp"
#include "common/CubePositionSerializer.hpp"

#include "tools/ByteArray.hpp"

namespace Server { namespace Network {

    void PacketExtractor::UdpReady(Tools::ByteArray const& p, bool& ready)
    {
        p.Read(ready);
    }

    void PacketExtractor::PassThrough(Tools::ByteArray const&)
    {
    }

    void PacketExtractor::PassThroughOk(Tools::ByteArray const&)
    {
    }

    void PacketExtractor::Login(Tools::ByteArray const& p,
                                Protocol::Version& major,
                                Protocol::Version& minor,
                                std::string& login)
    {
        p.Read(major);
        p.Read(minor);
        p.Read(login);
    }

    void PacketExtractor::Pong(Tools::ByteArray const& p,
                               Uint64& timestamp)
    {
        p.Read(timestamp);
    }

    void PacketExtractor::NeedChunks(Tools::ByteArray const& p,
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

    void PacketExtractor::GetNeededResourceIds(Tools::ByteArray const& p, Uint32& version)
    {
        p.Read(version);
    }

    void PacketExtractor::GetResourceRange(Tools::ByteArray const& p,
                                           Uint32& resourceId,
                                           Uint32& offset)
    {
        p.Read(resourceId);
        p.Read(offset);
    }

    void PacketExtractor::GetCubeType(Tools::ByteArray const& p, Game::Map::Chunk::CubeType& id)
    {
        p.Read(id);
    }

    void PacketExtractor::GetBodyType(Tools::ByteArray const& p, Uint32& id)
    {
        p.Read(id);
    }

    void PacketExtractor::Settings(Tools::ByteArray const& p,
                                   Uint32& viewDistance,
                                   std::string& playerName)
    {
        p.Read(viewDistance);
        p.Read(playerName);
    }

    void PacketExtractor::TeleportOk(Tools::ByteArray const&)
    {
    }

    void PacketExtractor::Move(Tools::ByteArray const& p, Common::MovingOrientedPosition& pos)
    {
        p.Read(pos);
    }

    void PacketExtractor::Action(Tools::ByteArray const& p, Common::OrientedPosition& pos, Common::CubePosition& cubePos, Uint32& actionId)
    {
        p.Read(pos);
        p.Read(cubePos);
        p.Read(actionId);
    }

    void PacketExtractor::EntityRemoved(Tools::ByteArray const& p, Uint32& entityId)
    {
        p.Read(entityId);
    }

}}
