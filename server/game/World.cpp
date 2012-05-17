#include "server/precompiled.hpp"

#include "server/game/World.hpp"
#include "server/game/Game.hpp"
#include "server/game/PluginManager.hpp"

#include "server/Server.hpp"
#include "server/rcon/ToJsonStr.hpp"

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

    void World::Save(Tools::Database::IConnection& conn)
    {
        Tools::debug << "World::Save()\n";
        for (auto it = this->_maps.begin(), ite = this->_maps.end(); it != ite; ++it)
            it->second->Save(conn);
    }

    std::string World::RconGetMaps() const
    {
        std::string json = "\t[\n";
        auto it = this->_maps.begin();
        auto itEnd = this->_maps.end();
        for (; it != itEnd; ++it)
        {
            if (it != this->_maps.begin())
                json += ",\n";
            json += "\t\t{\n"
                "\t\t\t\"identifier\": \"" + it->second->GetName() + "\",\n"
                "\t\t\t\"fullname\": \"" + Rcon::ToJsonStr(it->second->GetFullName()) + "\"\n"
                "\t\t}";
        }
        json += "\n\t]\n";
        return json;
    }

}}
