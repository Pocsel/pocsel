#include "server/precompiled.hpp"

#include "server/game/Game.hpp"
#include "server/game/Player.hpp"
#include "server/game/World.hpp"

#include "common/Position.hpp"
#include "common/OrientedPosition.hpp"
#include "common/MovingOrientedPosition.hpp"
#include "common/CubePosition.hpp"
#include "common/RayCast.hpp"

#include "tools/SimpleMessageQueue.hpp"

#include "server/Server.hpp"

#include "server/clientmanagement/ClientManager.hpp"

#include "server/game/map/Map.hpp"

namespace Server { namespace Game {

    Game::Game(Server& server, Tools::SimpleMessageQueue& messageQueue) :
        _server(server),
        _messageQueue(messageQueue)
    {
        Tools::debug << "Game::Game()\n";
        this->_world = new World(*this, this->_messageQueue);
    }

    Game::~Game()
    {
        Tools::debug << "Game::~Game()\n";
        Tools::Delete(this->_world);
    }

    void Game::Start()
    {
        Tools::debug << "Game::Start()\n";
        this->_world->Start();
    }

    void Game::Stop()
    {
        Tools::debug << "Game::Stop()\n";
        this->_world->Stop();
    }

    void Game::Save()
    {
        Tools::debug << "Game::Save()\n";
        this->_world->Save();
    }

    void Game::PlayerTeleportOk(Uint32 id)
    {
        auto it = this->_players.find(id);
        if (it == this->_players.end())
            return;
        Player* player = it->second.get();

        player->TeleportOk();
    }

    void Game::PlayerMove(Uint32 id, Common::MovingOrientedPosition const& pos)
    {
        auto it = this->_players.find(id);
        if (it == this->_players.end())
            return;
        Player* player = it->second.get();

        if (!player->IsInGame())
            return;

        player->GetCurrentMap().MovePlayer(id, pos);
    }

    void Game::PlayerAction(Uint32 id, Common::OrientedPosition const& pos, Common::CubePosition const& targetPos)
    {
        auto it = this->_players.find(id);
        if (it == this->_players.end())
            return;
        Player* player = it->second.get();

        if (!player->IsInGame())
            return;

        player->GetCurrentMap().DestroyCube(targetPos);
    }

    void Game::PlayerAction2(Uint32 id, Common::OrientedPosition const& pos, Common::CubePosition const& targetPos)
    {
        auto it = this->_players.find(id);
        if (it == this->_players.end())
            return;
        Player* player = it->second.get();

        if (!player->IsInGame())
            return;

        Common::Position p0(Common::Position(targetPos) +
                            glm::dvec3(0.5));

        float dist = (float)glm::distance(p0,pos.position);

        //player->GetCurrentMap().DestroyCubes(Common::RayCast::Ray(pos, 50.0f));
        if (dist > 1.0f)
            player->GetCurrentMap().DestroyCubes(Common::RayCast::SphereArea(p0, 100.0f / std::log10(dist * dist * 10.0f)));
    }

    void Game::SpawnPlayer(std::string const& clientName, Uint32 clientId, std::string const& playerName, Uint32 viewDistance)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&Game::_SpawnPlayer, this, std::cref(clientName), clientId, playerName, viewDistance));
        this->_messageQueue.PushMessage(m);
    }

    void Game::PlayerTeleport(Uint32 id, std::string const& map, Common::Position const& position)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&Game::_PlayerTeleport, this, id, std::cref(map), position));
        this->_messageQueue.PushMessage(m);
    }

    void Game::GetChunkPacket(Map::Chunk::IdType id, Uint32 clientId, ChunkPacketCallback& callback)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&Game::_GetChunkPacket, this, id, clientId, callback));
        this->_messageQueue.PushMessage(m);
    }

    void Game::RemovePlayer(Uint32 clientId)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&Game::_RemovePlayer, this, clientId));
        this->_messageQueue.PushMessage(m);
    }

    void Game::_SpawnPlayer(std::string const& clientName, Uint32 clientId, std::string const& playerName, Uint32 viewDistance)
    {
        // TODO dans le futur, clientName sera utilise pour un client connu par le serveur

        Player* newPlayer = new Player(*this, clientId, playerName, viewDistance);
        this->_players[clientId] = std::shared_ptr<Player>(newPlayer);

        Map::Map::SpawnCallback cb(std::bind(&Game::PlayerTeleport,
                                             this,
                                             clientId,
                                             std::cref(this->_world->GetDefaultMap().GetName()),
                                             std::placeholders::_1));
        this->_world->GetDefaultMap().GetSpawnPosition(cb);
    }

    void Game::_PlayerTeleport(Uint32 id, std::string const& mapName, Common::Position const& position)
    {
        auto it = this->_players.find(id);
        if (it == this->_players.end())
            return;
        Player* player = it->second.get();

        Map::Map* map;

        if (this->_world->HasMap(mapName))
            map = &this->_world->GetMap(mapName);
        else
            map = &this->_world->GetDefaultMap();

        player->Teleport(*map, position);

        this->_server.GetClientManager().ClientTeleport(id, std::cref(map->GetName()), position);
    }

    void Game::_GetChunkPacket(Map::Chunk::IdType id, Uint32 clientId, ChunkPacketCallback& callback)
    {
        auto it = this->_players.find(clientId);
        if (it == this->_players.end())
            return;
        Player* player = it->second.get();

        if (!player->HasMap())
            return;
        player->GetCurrentMap().GetChunkPacket(id, callback);
    }

    void Game::_RemovePlayer(Uint32 clientId)
    {
        auto it = this->_players.find(clientId);
        if (it == this->_players.end())
            return;
        Player* player = it->second.get();

        player->RemoveFromMap();
        this->_players.erase(clientId);
    }

}}
