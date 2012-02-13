#include "server/game/World.hpp"

#include "server/Server.hpp"

#include "server/database/WorldLoader.hpp"

#include "server/game/map/Map.hpp"

namespace Server { namespace Game{

    World::World(Server& server, Tools::SimpleMessageQueue& gameMessageQueue) :
        _server(server),
        _messageQueue(gameMessageQueue)
    {
        Tools::debug << "World::World()\n";
        try
        {
            Database::WorldLoader::Load(*this, server.GetResourceManager());
        }
        catch (std::exception& e)
        {
            Tools::error << "Could not load world: " << e.what() << "\n";
            throw;
        }
    }

    World::~World()
    {
        Tools::debug << "World::~World()\n";

        for (auto it = this->_maps.begin(), ite = this->_maps.end(); it != ite; ++it)
            Tools::Delete(it->second);
    }

    void World::Start()
    {
        Tools::debug << "World::Start()\n";
        for (auto it = this->_maps.begin(), ite = this->_maps.end(); it != ite; ++it)
            it->second->Start();
    }

    void World::Stop()
    {
        Tools::debug << "World::Stop()\n";
        for (auto it = this->_maps.begin(), ite = this->_maps.end(); it != ite; ++it)
            it->second->Stop();
    }

}}
