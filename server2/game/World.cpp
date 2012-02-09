#include "server2/game/World.hpp"

#include "server2/Server.hpp"

#include "server2/database/WorldLoader.hpp"

namespace Server { namespace Game{

    World::World(Server& server) :
        _server(server)
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
    }

    Map::Map* World::GetMap(std::string const& name)
    {
        if (this->_maps.find(name) == this->_maps.end())
            return 0;
        return this->_maps[name];
    }

}}
