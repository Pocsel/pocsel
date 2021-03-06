#ifndef __SERVER_GAME_WORLD_HPP__
#define __SERVER_GAME_WORLD_HPP__

#include "common/CubeType.hpp"

#include "tools/SimpleMessageQueue.hpp"

namespace Server {
    class Server;
    namespace Game { namespace Map {
        class Map;
    }}
    namespace Database {
        class WorldLoader;
        class WorldLoader2;
    }
}
namespace Tools { namespace Database {
    class IConnection;
}}

namespace Server { namespace Game {

    class Game;
    class PluginManager;

    class World :
        private boost::noncopyable
    {
    public:
        boost::mutex saveMutex;

    private:
        Game& _game;
        Tools::SimpleMessageQueue& _messageQueue;
        std::unordered_map<std::string, Map::Map*> _maps;
        Map::Map* _defaultMap;
        std::string _identifier;
        std::string _fullname;
        Uint32 _version;
        std::string _buildHash;
        PluginManager* _pluginManager;

    public:
        World(Game& game, Tools::SimpleMessageQueue& gameMessageQueue);
        ~World();

        bool HasMap(std::string const& name) { return this->_maps.find(name) != this->_maps.end(); }
        Map::Map& GetMap(std::string const& name) { assert(this->_maps.find(name) != this->_maps.end()); return *this->_maps[name]; }
        Map::Map& GetDefaultMap() const { return *this->_defaultMap; }
        PluginManager& GetPluginManager() { return *this->_pluginManager; }
        std::unordered_map<std::string, Map::Map*> const& GetMaps() const { return this->_maps; }
        Game& GetGame() { return this->_game; }

        void Start();
        void Stop();
        void Save(Tools::Database::IConnection& conn);

        // Appelable d'un autre thread, que des trucs const qui ne peuvent pas disparaitre
        std::string const& GetIdentifier() const { return this->_identifier; }
        std::string const& GetFullname() const { return this->_fullname; }
        Uint32 GetVersion() const { return this->_version; }
        std::string const& GetBuildHash() const { return this->_buildHash; }

        std::string RconGetMaps() const;

    private:
        friend class Database::WorldLoader;
        friend class Database::WorldLoader2;
    };

}}

#endif
