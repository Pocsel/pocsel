#include "server/game/Game.hpp"
#include "server/game/Player.hpp"
#include "server/game/World.hpp"

#include "server/Server.hpp"

#include "server/clientmanagement/ClientManager.hpp"

#include "server/game/map/Map.hpp"

namespace Server { namespace Game {

    Game::Game(Server& server) :
        _server(server),
        _messageQueue(1)
    {
        Tools::debug << "Game::Game()\n";
        this->_world = new World(server, this->_messageQueue);
    }

    Game::~Game()
    {
        Tools::debug << "Game::~Game()\n";
        Tools::Delete(this->_world);

        for (auto it = this->_players.begin(), ite = this->_players.end(); it != ite; ++it)
            Tools::Delete(it->second);
    }

    void Game::Start()
    {
        Tools::debug << "Game::Start()\n";
        this->_world->Start();
        this->_messageQueue.Start();
    }

    void Game::Stop()
    {
        Tools::debug << "Game::Stop()\n";
        this->_world->Stop();
        this->_messageQueue.Stop();
    }

    void Game::_SpawnPlayer(std::string const& clientName, Uint32 clientId)
    {
        // dans le futur, clientName sera utilise pour un client connu par le serveur

        Player* newPlayer = new Player(*this, clientId);
        this->_players[clientId] = newPlayer;

        newPlayer->SetCurrentMap(this->_world->GetDefaultMap());

        this->_world->GetDefaultMap().GetSpawnPosition(
            std::bind(&Game::PlayerTeleport, this, clientId, std::placeholders::_1)
            );
    }

    void Game::_PlayerTeleport(Uint32 id, Common::Position const& position)
    {
        auto it = this->_players.find(id);
        if (it == this->_players.end())
            return;
        Player* player = it->second;
        player->Teleport(position);
        this->_server.GetClientManager().ClientTeleport(id, position);
    }

    void Game::_GetChunk(Chunk::IdType id, Uint32 clientId, ChunkCallback callback)
    {
        static std::unordered_map<Uint32, Uint32> __;

        this->_world->GetDefaultMap().GetChunk(id, callback);
        return;
        // TODO OBSOLETE virer les 2 lignes au dessus, qui sont pour le vieux protocole

        auto it = this->_players.find(clientId);
        if (it == this->_players.end())
            return;
        Player* player = it->second;
        if (!player->HasMap())
        {
            // kick player ?
            return;
        }
        player->GetCurrentMap().GetChunk(id, callback);
    }

}}
