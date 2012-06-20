#ifndef __SERVER_DATABASE_WORLDLOADER2_HPP__
#define __SERVER_DATABASE_WORLDLOADER2_HPP__

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

    class WorldLoader2
    {
    private:
        ResourceManager& _resourceManager;
        Tools::Database::IConnection& _conn;

        // ordre de remplissage
        Game::World& _world;

    public:
        WorldLoader2(Game::World& world, ResourceManager& resourceManager);

    private:
        // ordre d'appellage
        void _LoadMetadata();
        void _LoadPlugins();
        void _LoadMaps();

        Common::BaseChunk::CubeType _GetCubeTypeIdByName(Uint32 pluginId, std::string const& name);
        Game::Map::CubeType const* _GetCubeTypeByName(Uint32 pluginId, std::string const& name);
    };

}}

#endif
