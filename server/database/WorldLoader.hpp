#ifndef __SERVER_DATABASE_WORLDLOADER_HPP__
#define __SERVER_DATABASE_WORLDLOADER_HPP__

namespace Common {
    struct CubeType;
}

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
        class Map;
    }
}}

namespace Server { namespace Database {

    class ResourceManager;

    class WorldLoader
    {
    public:
        static void Load(Game::World& world, ResourceManager& manager);

    private:
        static void _RegisterResourcesFunctions(Game::Map::Map& map, Tools::Lua::Interpreter& interpreter);
        static void _LoadCubeTypes(Tools::Database::IConnection& conn, Game::Map::Map& map, ResourceManager& manager);
        static void _LoadMapConf(Game::Map::Conf& conf,
                                 std::string const& name,
                                 std::string const& code,
                                 Game::World const& world);
        static Common::CubeType const* _GetCubeTypeByName(std::string const& name,
                                                          Game::World const& world);
    };

}}

#endif
