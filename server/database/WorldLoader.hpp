#ifndef __SERVER_DATABASE_WORLDLOADER_HPP__
#define __SERVER_DATABASE_WORLDLOADER_HPP__

#include "tools/lua/Ref.hpp"
#include "common/BaseChunk.hpp"

namespace Tools {
    namespace Database {
        class IConnection;
    }
    namespace Lua {
        class Interpreter;
    }
}

namespace Server { namespace Game {
    class World;
    namespace Map {
        struct Conf;
        struct CubeType;
        class Map;
    }
}}

namespace Server { namespace Database {

    class ResourceManager;

    class WorldLoader
    {
    private:
        struct LoadingMapConf
        {
            Tools::Lua::Interpreter* interpreter;
            Tools::Lua::Ref fullname;
            Tools::Lua::Ref isDefault;
            Tools::Lua::Ref cubes;
            Tools::Lua::Ref equations;

            LoadingMapConf(Tools::Lua::Interpreter* interpreter,
                Tools::Lua::Ref fullname,
                Tools::Lua::Ref isDefault,
                Tools::Lua::Ref cubes,
                Tools::Lua::Ref equations);
            LoadingMapConf(LoadingMapConf&& rhs);
            ~LoadingMapConf();
        };

        Game::World& _world;
        ResourceManager& _resourceMgr;
        Tools::Database::IConnection& _connection;
        std::map<Game::Map::Map*, LoadingMapConf> _loadingMaps;

    public:
        WorldLoader(Game::World& world, ResourceManager& manager);

    private:
        void _RegisterResourcesFunctions(Game::Map::Map& map, Tools::Lua::Interpreter& interpreter);
        void _LoadCubeTypes(Game::Map::Map& map);
        LoadingMapConf _LoadMapConf(Game::Map::Conf& conf, std::string const& name, std::string const& code);
        void _LoadMapCubeTypes();
        Game::Map::CubeType const* _GetCubeTypeByName(std::string const& name);
        Common::BaseChunk::CubeType _GetCubeTypeId(Uint32 pluginId, std::string const& name);
    };

}}

#endif
