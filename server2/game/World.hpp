#ifndef __SERVER_GAME_WORLD_HPP__
#define __SERVER_GAME_WORLD_HPP__

#include "common/CubeType.hpp"

namespace Server {

    class Server;

    namespace Database {

        class WorldLoader;

    }

}

namespace Server { namespace Game {

    namespace Map {

        class Map;

    }

    class World :
        private boost::noncopyable
    {
    private:
        Server& _server;
        std::unordered_map<std::string, Map::Map*> _maps;
        mutable Map::Map* _defaultMap; // TODO retirer mutable
        std::vector<Common::CubeType> _cubeTypes;
        std::string _identifier;
        std::string _fullname;
        Uint32 _version;

    public:
        World(Server& server);
        ~World();

        Map::Map* GetMap(std::string const& name);
        Map::Map* GetDefaultMap() const { return this->_defaultMap; } // TODO retirer const

        void Start();
        void Stop();

        // Appelable d'un autre thread, que des trucs const qui ne peuvent pas disparaitre
        std::vector<Common::CubeType> const& GetCubeTypes() const { return this->_cubeTypes; }
        Common::CubeType const& GetCubeType(Uint32 id) const { assert(id != 0); return this->_cubeTypes[id - 1]; }
        bool HasCubeType(Uint32 id) const { return id && this->_cubeTypes.size() >= id; }
        std::string const& GetIdentifier() const { return this->_identifier; }
        std::string const& GetFullname() const { return this->_fullname; }
        Uint32 GetVersion() const { return this->_version; }

    private:
        friend class Database::WorldLoader;
    };

}}

#endif
