#include "client2/precompiled.hpp"
#include "common/Packet.hpp"
#include "client2/game/Game.hpp"
#include "client2/game/CubeTypeManager.hpp"
#include "client2/map/Map.hpp"
#include "client2/map/ChunkManager.hpp"
#include "client2/resources/ResourceDownloader.hpp"
#include "client2/network/Network.hpp"
#include "client2/network/PacketCreator.hpp"
#include "client2/network/PacketDispatcher.hpp"
#include "client2/network/PacketExtractor.hpp"
#include "client2/Client.hpp"

namespace Client { namespace Network {

    PacketDispatcher::PacketDispatcher(Client& client)
        : _client(client)
    {
        // Network
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::LoggedIn] = std::bind(&PacketDispatcher::_HandleLogin, this, std::placeholders::_1);
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::Ping] =
            [this](Common::Packet& p)
            {
                this->_client.GetNetwork().SendPacket(PacketCreator::Pong(PacketExtractor::ExtractPing(p)));
            };

        // Loading
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::NeededResourceIds] = std::bind(&PacketDispatcher::_HandleNeededResourceIds, this, std::placeholders::_1);
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::ResourceRange] =
            [this](Common::Packet& p)
            {
                if (this->_client.GetState() != Client::LoadingResources)
                    throw std::runtime_error("Bad state for loading a resource");
                this->_client.GetGame().GetResourceManager().GetDownloader().HandleResourceRange(p);
            };
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::CubeType] =
            [this](Common::Packet& p)
            {
                if (this->_client.GetState() != Client::LoadingResources)
                    throw std::runtime_error("Bad state for loading a cube type");
                this->_client.GetGame().GetCubeTypeManager().AddCubeType(PacketExtractor::ExtractCubeType(p));
            };

        // Game
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::Chunk] =
            [this](Common::Packet& p)
            {
                if (this->_client.GetState() != Client::LoadingChunks && this->_client.GetState() != Client::Running)
                    throw std::runtime_error("Bad state for loading a chunk");
                this->_client.GetGame().GetMap().GetChunkManager().AddChunk(PacketExtractor::ExtractChunk(p));
            };
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::TeleportPlayer] =
            [this](Common::Packet& p)
            {
                if (this->_client.GetState() != Client::LoadingChunks && this->_client.GetState() != Client::Running)
                    throw std::runtime_error("Bad state for teleporting");
                std::string map;
                Common::Position position;
                PacketExtractor::ExtractTeleportPlayer(p, map, position);
                this->_client.GetGame().TeleportPlayer(map, position);
            };
    }

    void PacketDispatcher::ProcessAllPackets(std::list<Common::Packet*>&& packets)
    {
        for (auto it = packets.begin(), ite = packets.end(); it != ite; ++it)
        {
            this->_ProcessPacket(**it);
            delete *it;
        }
    }

    void PacketDispatcher::_ProcessPacket(Common::Packet& packet)
    {
        Protocol::ActionType id;
        packet.Read(id);
        this->_dispatcher[id](packet);
    }

    void PacketDispatcher::_HandleLogin(Common::Packet& p)
    {
        bool status;
        Protocol::Version major, minor;
        std::string reason, worldId, worldName;
        Uint32 worldVersion;
        Common::BaseChunk::CubeType nbCubeTypes;
        PacketExtractor::ExtractLogin(p, status, major, minor, reason, worldId, worldName, worldVersion, nbCubeTypes);

        Tools::debug << "LoggedIn: " << (status ? "ok" : "ko") << " Protocol: " << major << "." << minor << "\n";
        if (status)
        {
            Tools::debug << "World: " << worldName << "(" << worldId << " version: " << worldVersion << ")\n";
            this->_client.Login(worldId, worldName, worldVersion, nbCubeTypes);
        }
        else
            this->_client.Disconnect(reason);
    }

    void PacketDispatcher::_HandleNeededResourceIds(Common::Packet& p)
    {
        if (this->_client.GetState() != Client::LoadingResources)
            return;
        Uint32 nbNeededResources;
        std::list<Uint32> neededResourceIds;
        PacketExtractor::ExtractNeededResourceIds(p, nbNeededResources, neededResourceIds);
        this->_client.GetGame().GetResourceManager().GetDownloader().AskResources(nbNeededResources, neededResourceIds);
    }

}}
