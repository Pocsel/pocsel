#include "client/precompiled.hpp"
#include "common/MovingOrientedPosition.hpp"
#include "common/physics/Node.hpp"
#include "tools/ByteArray.hpp"
#include "client/game/Game.hpp"
#include "client/game/CubeTypeManager.hpp"
#include "client/game/engine/BodyType.hpp"
#include "client/game/engine/BodyManager.hpp"
#include "client/map/Map.hpp"
#include "client/map/ChunkManager.hpp"
#include "client/resources/ResourceDownloader.hpp"
#include "client/network/Network.hpp"
#include "client/network/PacketCreator.hpp"
#include "client/network/PacketDispatcher.hpp"
#include "client/network/PacketExtractor.hpp"
#include "client/Client.hpp"

namespace Client { namespace Network {

    PacketDispatcher::PacketDispatcher(Client& client)
        : _client(client)
    {
        // Network
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::LoggedIn] = std::bind(&PacketDispatcher::_HandleLogin, this, std::placeholders::_1);
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::Ping] =
            [this](Tools::ByteArray& p)
            {
                this->_client.GetNetwork().SendPacket(PacketCreator::Pong(PacketExtractor::Ping(p)));
            };

        // Loading
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::NeededResourceIds] = std::bind(&PacketDispatcher::_HandleNeededResourceIds, this, std::placeholders::_1);
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::ResourceRange] =
            [this](Tools::ByteArray& p)
            {
                if (this->_client.GetState() != Client::LoadingResources)
                    throw std::runtime_error("Bad state for loading a resource");
                this->_client.GetGame().GetResourceManager().GetDownloader().HandleResourceRange(p);
            };
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::CubeType] =
            [this](Tools::ByteArray& p)
            {
                if (this->_client.GetState() != Client::LoadingResources)
                    throw std::runtime_error("Bad state for loading a cube type");
                this->_client.GetGame().GetCubeTypeManager().AddCubeType(PacketExtractor::CubeType(p));
            };
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::BodyType] =
            [this](Tools::ByteArray& p)
            {
                if (this->_client.GetState() != Client::LoadingResources)
                    throw std::runtime_error("Bad state for loading a body type");
                this->_client.GetGame().GetEngine().GetBodyManager().AddBodyType(PacketExtractor::BodyType(p));
            };

        // Game
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::Chunk] =
            [this](Tools::ByteArray& p)
            {
                if (this->_client.GetState() != Client::LoadingChunks && this->_client.GetState() != Client::Running)
                    throw std::runtime_error("Bad state for loading a chunk");
                this->_client.GetGame().GetMap().GetChunkManager().AddChunk(PacketExtractor::Chunk(p));
            };
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::TeleportPlayer] =
            [this](Tools::ByteArray& p)
            {
                if (this->_client.GetState() != Client::WaitingPosition &&
                    this->_client.GetState() != Client::LoadingChunks &&
                    this->_client.GetState() != Client::Running)
                    throw std::runtime_error("Bad state for teleporting");
                std::string map;
                Common::Position position;
                PacketExtractor::TeleportPlayer(p, map, position);
                this->_client.GetGame().TeleportPlayer(map, position);
            };
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::DoodadSpawn] =
            [this](Tools::ByteArray& p)
            {
                if (this->_client.GetState() != Client::LoadingChunks &&
                    this->_client.GetState() != Client::Running)
                    throw std::runtime_error("Bad state for doodad spawn");
                Uint32 doodadId;
                Uint32 entityId;
                Uint32 bodyId;
                std::string doodadName;
                Common::Physics::Node position;
                std::list<std::pair<std::string /* key */, std::string /* value */>> values;
                PacketExtractor::DoodadSpawn(p,
                        doodadId,
                        entityId,
                        doodadName,
                        position,
                        bodyId,
                        values);
                this->_client.GetGame().GetEngine().GetDoodadManager().SpawnDoodad(
                        doodadId,
                        entityId,
                        bodyId,
                        doodadName,
                        position,
                        values);
            };
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::DoodadKill] =
            [this](Tools::ByteArray& p)
            {
                if (this->_client.GetState() != Client::LoadingChunks &&
                    this->_client.GetState() != Client::Running)
                    throw std::runtime_error("Bad state for doodad kill");
                Uint32 doodadId;
                PacketExtractor::DoodadKill(p, doodadId);
                this->_client.GetGame().GetEngine().GetDoodadManager().KillDoodad(doodadId);
            };
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::DoodadUpdate] =
            [this](Tools::ByteArray& p)
            {
                if (this->_client.GetState() != Client::LoadingChunks &&
                    this->_client.GetState() != Client::Running)
                    throw std::runtime_error("Bad state for doodad update");
                Uint32 doodadId;
                std::unique_ptr<std::vector<std::pair<bool, Common::Physics::Node>>> body;
                std::list<std::tuple<bool, std::string /* key */, std::string /* value */>> commands;
                PacketExtractor::DoodadUpdate(p, doodadId, body, commands);
                this->_client.GetGame().GetEngine().GetDoodadManager().UpdateDoodad(doodadId, body.get(), commands);
            };
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::EntityUpdate] =
            [this](Tools::ByteArray& p)
            {
                if (this->_client.GetState() != Client::LoadingChunks &&
                    this->_client.GetState() != Client::Running)
                    throw std::runtime_error("Bad state for entity update");

                Uint32 entityId;
                std::vector<Common::Physics::Node> physics;
                PacketExtractor::EntityUpdate(p, entityId, physics);

                this->_client.GetGame().GetEngine().GetDoodadManager().UpdateEntity(entityId, physics);
            };
    }

    void PacketDispatcher::ProcessAllPackets(std::list<Tools::ByteArray*>&& packets)
    {
        for (auto it = packets.begin(), ite = packets.end(); it != ite; ++it)
        {
            this->_ProcessPacket(**it);
            Tools::Delete(*it);
        }
    }

    void PacketDispatcher::_ProcessPacket(Tools::ByteArray& packet)
    {
        Protocol::ActionType id;
        packet.Read(id);
        this->_dispatcher[id](packet);
    }

    void PacketDispatcher::_HandleLogin(Tools::ByteArray& p)
    {
        bool status;
        Protocol::Version major, minor;
        std::string reason, worldId, worldName, worldBuildHash;
        Uint32 clientId;
        Uint32 worldVersion;
        Common::BaseChunk::CubeType nbCubeTypes;
        Uint32 nbBodyTypes;
        PacketExtractor::Login(p, status, major, minor, reason, clientId, worldId, worldName, worldVersion, nbCubeTypes, nbBodyTypes, worldBuildHash);

        Tools::debug << "LoggedIn: " << (status ? "ok" : "ko") << " Protocol: " << static_cast<int>(major) << "." << static_cast<int>(minor) << "\n";
        if (status)
        {
            Tools::debug << "World: " << worldName << " (identifier: \"" << worldId << "\", version: " << worldVersion << ", build hash: \"" << worldBuildHash << "\")\n";
            this->_client.Login(clientId, worldId, worldName, worldVersion, nbCubeTypes, nbBodyTypes, worldBuildHash);
        }
        else
            this->_client.Disconnect(reason);
    }

    void PacketDispatcher::_HandleNeededResourceIds(Tools::ByteArray& p)
    {
        if (this->_client.GetState() != Client::LoadingResources)
            return;
        Uint32 nbNeededResources;
        std::list<Uint32> neededResourceIds;
        PacketExtractor::NeededResourceIds(p, nbNeededResources, neededResourceIds);
        this->_client.GetGame().GetResourceManager().GetDownloader().AskResources(nbNeededResources, neededResourceIds);
    }

}}
