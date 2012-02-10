#ifndef __SERVER_DATABASE_WORLDLOADER_HPP__
#define __SERVER_DATABASE_WORLDLOADER_HPP__

namespace Common {

    struct CubeType;

}

namespace Server { namespace Game {

    class World;

    namespace Map {

        struct Conf;

    }

}}

namespace Server { namespace Database {

    class ResourceManager;

    class WorldLoader
    {
    public:
        static void Load(Game::World& world, ResourceManager& manager);

    private:
        static void _LoadCubeType(Common::CubeType& descr,
                                  std::string const& code,
                                  ResourceManager const& manager);
        static void _LoadMapConf(Game::Map::Conf& conf,
                                 std::string const& name,
                                 std::string const& code,
                                 Game::World const& world);
        static Common::CubeType const* _GetCubeTypeByName(std::string const& name,
                                                          Game::World const& world);
    };

}}

#endif
