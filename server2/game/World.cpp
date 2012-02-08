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
            std::cout << "Could not load world: " << e.what() << "\n";
            throw;
        }
    }

    World::~World()
    {
    }

}}

