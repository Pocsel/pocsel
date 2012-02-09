#include "server2/game/World.hpp"

#include "server2/Server.hpp"

#include "server2/database/WorldLoader.hpp"

namespace Server { namespace Game{

    World::World(Server& server) :
        _server(server)
    {
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
    }

    Map::Map* World::GetMap(std::string const& name)
    {
        if (this->_maps.find(name) == this->_maps.end())
            return 0;
        return this->_maps[name];
    }

}}
