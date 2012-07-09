#include "client/precompiled.hpp"

#include "client/network/ChunkSerializer.hpp"
#include "client/network/BodyTypeSerializer.hpp"
#include "client/network/BodySerializer.hpp"
#include "client/network/PacketExtractor.hpp"
#include "client/game/engine/BodyType.hpp"
#include "common/Packet.hpp"
#include "common/CubeTypeSerializer.hpp"
#include "common/MovingOrientedPositionSerializer.hpp"
#include "common/physics/NodeSerializer.hpp"

namespace Client { namespace Network {

    void PacketExtractor::Login(Tools::ByteArray const& p,
            bool& status,
            Protocol::Version& major,
            Protocol::Version& minor,
            std::string& reason,
            Uint32& clientId,
            std::string& worldIdentifier,
            std::string& worldName,
            Uint32& worldVersion,
            Common::BaseChunk::CubeType& nbCubeTypes,
            Uint32& nbBodyTypes,
            std::string& worldBuildHash)
    {
        p.Read(status);
        p.Read(major);
        p.Read(minor);
        if (status == false)
            p.Read(reason);
        else
        {
            p.Read(clientId);
            p.Read(worldIdentifier);
            p.Read(worldName);
            p.Read(worldVersion);
            p.Read(nbCubeTypes);
            p.Read(nbBodyTypes);
            p.Read(worldBuildHash);
            if (nbCubeTypes == 0)
                throw std::runtime_error("nbCubeTypes == 0");
        }
    }

    Uint64 PacketExtractor::Ping(Tools::ByteArray const& p)
    {
        return p.Read64();
    }

    void PacketExtractor::NeededResourceIds(Tools::ByteArray const& p, Uint32& nbResourceIds, std::list<Uint32>& ids)
    {
        p.Read(nbResourceIds);
        unsigned int nbIds = p.GetBytesLeft() / sizeof(Uint32);
        for (unsigned int i = 0; i < nbIds; ++i)
            ids.push_back(p.Read32());
    }

    char const* PacketExtractor::ResourceRange(Tools::ByteArray const& p,
                                                      Uint32& id,
                                                      Uint32& offset,
                                                      std::string& type,
                                                      std::string& filename,
                                                      Uint32& totalSize,
                                                      Uint32& dataSize)
    {
        p.Read(id);
        p.Read(offset);
        if (!offset)
        {
            p.Read(type);
            p.Read(filename);
            p.Read(totalSize);
        }
        dataSize = p.GetBytesLeft();
        return p.ReadRawData(dataSize);
    }

    std::unique_ptr<Map::Chunk> PacketExtractor::Chunk(Tools::ByteArray const& p)
    {
        return p.Read<Map::Chunk>();
    }

    std::unique_ptr<Common::CubeType> PacketExtractor::CubeType(Tools::ByteArray const& p)
    {
        return p.Read<Common::CubeType>();
    }

    std::unique_ptr<Client::Game::Engine::BodyType> PacketExtractor::BodyType(Tools::ByteArray const& p)
    {
        return p.Read<Client::Game::Engine::BodyType>();
    }

    void PacketExtractor::TeleportPlayer(Tools::ByteArray const& p, std::string& map, Common::Position& position)
    {
        p.Read(map);
        p.Read(position);
    }

    void PacketExtractor::DoodadSpawn(Tools::ByteArray const& p,
            Uint32& doodadId,
            Uint32& entityId,
            std::string& doodadName,
            Common::Physics::Node& position,
            Uint32& bodyId,
            std::list<std::pair<std::string /* key */, std::string /* value */>>& values)
    {
        p.Read(doodadId);
        p.Read(entityId);
        p.Read(doodadName);
        p.Read(position);
        p.Read(bodyId);
        while (p.GetBytesLeft())
            values.push_back(std::make_pair(p.ReadString(), p.ReadString()));
    }

    void PacketExtractor::DoodadKill(Tools::ByteArray const& p,
            Uint32& doodadId)
    {
        p.Read(doodadId);
    }

    void PacketExtractor::DoodadUpdate(Tools::ByteArray const& p,
            Uint32& doodadId,
            std::unique_ptr<Client::Game::Engine::Body>& body,
            std::list<std::tuple<bool /* functionCall */, std::string /* function || key */, std::string /* value */>>& commands)
    {
        p.Read(doodadId);
        if (p.ReadBool())
            body = p.Read<Client::Game::Engine::Body>();
        while (p.GetBytesLeft())
        {
            bool functionCall = p.ReadBool();
            std::string functionOrKey = p.ReadString();
            std::string value = p.ReadString();
            commands.push_back(std::make_tuple(functionCall, functionOrKey, value));
        }
    }

    void PacketExtractor::EntityUpdate(Tools::ByteArray const& p,
            Uint32 entityId,
            Common::Physics::Node& node)
    {
        p.Read(entityId);
        p.Read(node);
    }

}}
