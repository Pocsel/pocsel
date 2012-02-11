#include "server2/game/World.hpp"

#include "server2/Server.hpp"

#include "server2/database/WorldLoader.hpp"

#include "server2/game/map/Map.hpp"

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

    Map::Map* World::GetMap(std::string const& name)
    {
        if (this->_maps.find(name) == this->_maps.end())
            return 0;
        return this->_maps[name];
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
