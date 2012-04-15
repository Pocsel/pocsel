#include "server/precompiled.hpp"

#include "server/game/World.hpp"
#include "server/game/Game.hpp"
#include "server/game/PluginManager.hpp"

#include "server/Server.hpp"

#include "server/database/WorldLoader.hpp"

#include "server/game/map/Map.hpp"

namespace Server { namespace Game{

    World::World(Game& game, Tools::SimpleMessageQueue& gameMessageQueue) :
        _game(game),
        _messageQueue(gameMessageQueue)
    {
        Tools::debug << "World::World()\n";
        this->_pluginManager = new PluginManager();
        try
        {
            Database::WorldLoader(*this, game.GetServer().GetResourceManager());
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

        Tools::Delete(this->_pluginManager);
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

    void World::Save()
    {
        Tools::debug << "World::Save()\n";
        for (auto it = this->_maps.begin(), ite = this->_maps.end(); it != ite; ++it)
        {
            it->second->Save();
        }
    }

}}
