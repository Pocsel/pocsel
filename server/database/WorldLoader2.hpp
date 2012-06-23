#ifndef __SERVER_DATABASE_WORLDLOADER2_HPP__
#define __SERVER_DATABASE_WORLDLOADER2_HPP__

#include "common/BaseChunk.hpp"

namespace Tools {
    namespace Database {
        class IConnection;
    }
    namespace Lua {
        class Interpreter;
        class CallHelper;
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

    class WorldLoader2
    {
    private:
        ResourceManager& _resourceManager;
        Tools::Database::IConnection& _conn;
        Game::World& _world;
        std::list<std::pair<std::string /* map name */, Tools::Lua::Interpreter*>> _tmpMaps; // maps en cours de chargement
        Game::Map::Map* _currentMap; // map en cours de processing de server_file

    public:
        WorldLoader2(Game::World& world, ResourceManager& resourceManager);
        ~WorldLoader2();

    private:
        // ordre d'appellage
        void _LoadMetadata();
        void _LoadPlugins();
        void _LoadMaps();
        void _ParseMapConf(std::string const& name, std::string const& lua, Game::Map::Conf& conf); // ajoute un interpreter dans _tmpMaps pour gerer les cubes apres
        void _LoadServerFiles();
        void _ApiRegisterCube(Tools::Lua::CallHelper& helper);
        void _LoadCubeTypes();

        Common::BaseChunk::CubeType _GetCubeTypeIdByName(Uint32 pluginId, std::string const& name);
        Game::Map::CubeType const* _GetCubeTypeByName(Uint32 pluginId, std::string const& name);
    };

}}

#endif
