#include "server/database/WorldLoader.hpp"
#include "server/database/ResourceManager.hpp"

#include "tools/database/IConnectionPool.hpp"

#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Iterator.hpp"

#include "server/game/World.hpp"

#include "server/game/map/Conf.hpp"
#include "server/game/map/Map.hpp"

#include "server/Logger.hpp"

namespace Server { namespace Database {

    void WorldLoader::Load(Game::World& world, ResourceManager& manager)
    {
        auto conn = manager.GetConnectionPool().GetConnection();
        auto& curs = conn->GetCursor();

        // Meta data
        curs.Execute("SELECT identifier, fullname, version, build_hash FROM world");
        if (curs.HasData())
        {
            auto& row = curs.FetchOne();
            world._identifier = row[0].GetString();
            world._fullname = row[1].GetString();
            world._version = row[2].GetInt();
            world._buildHash = row[3].GetString();
        }
        else
            throw std::runtime_error("World file missing metadata.");

        // Cube types
        curs.Execute("SELECT id, plugin_id, name, lua FROM cube_type");
        while (curs.HasData())
        {
            auto& row = curs.FetchOne();
            Uint32 id = row[0].GetUint64();
            std::string name = row[2].GetString();
            Log::load << "Load cube type id: " << id <<
                          ", plugin_id: " << row[1].GetUint64() <<
                          ", name: " << row[2].GetString() << "\n";
            if (world._cubeTypes.size() < id)
                world._cubeTypes.resize(id);
            world._cubeTypes[id - 1].id = id;
            world._cubeTypes[id - 1].name = name;
            WorldLoader::_LoadCubeType(world._cubeTypes[id - 1], row[3].GetString(), manager);
        }

        // Maps
        curs.Execute("SELECT name, lua FROM map");
        while (curs.HasData())
        {
            auto& row = curs.FetchOne();
            try
            {
                Game::Map::Conf conf;
                WorldLoader::_LoadMapConf(conf, row[0].GetString(), row[1].GetString(), world);
                conf.cubeTypes = &world._cubeTypes;
                world._maps[conf.name] = new Game::Map::Map(conf, world._game);
                if (conf.is_default) {
                    world._defaultMap = world._maps[conf.name];
                }
            }
            catch (std::exception& e)
            {
                Log::load << "WorldLoader: Failed to load map \"" << row[0].GetString() << "\": " << e.what() << Tools::endl;
                Tools::error << "WorldLoader: Failed to load map \"" << row[0].GetString() << "\": " << e.what() << Tools::endl;
            }
        }
        if (world._defaultMap == 0)
            throw std::runtime_error("Cannot find default map");
    }

    void WorldLoader::_LoadCubeType(Common::CubeType& descr, std::string const& code, ResourceManager const& manager)
    {
        try
        {
            Tools::Lua::Interpreter lua;
            lua.DoString(code);

            auto textures = lua.Globals()["textures"];
            descr.textures.top = manager.GetId(textures["top"].To<std::string>());
            descr.textures.left = manager.GetId(textures["left"].To<std::string>());
            descr.textures.front = manager.GetId(textures["front"].To<std::string>());
            descr.textures.right = manager.GetId(textures["right"].To<std::string>());
            descr.textures.back = manager.GetId(textures["back"].To<std::string>());
            descr.textures.bottom = manager.GetId(textures["bottom"].To<std::string>());

            descr.solid = lua.Globals()["solid"].To<bool>();
            descr.transparent = lua.Globals()["transparent"].To<bool>();

            Log::load << "name: " << descr.name << " " << descr.id << "\n";
            Log::load << "textures.top: " << descr.textures.top << "\n";
            Log::load << "textures.left: " << descr.textures.left << "\n";
            Log::load << "textures.front: " << descr.textures.front << "\n";
            Log::load << "textures.right: " << descr.textures.right << "\n";
            Log::load << "textures.back: " << descr.textures.back << "\n";
            Log::load << "textures.bottom: " << descr.textures.bottom << "\n";

            Log::load << "solid: " << descr.solid << "\n";
            Log::load << "transparent: " << descr.transparent << "\n";

            Log::load << "\n";
        }
        catch (std::exception& e)
        {
            Tools::error << "WorldLoader: " << e.what();
            Log::load << "WorldLoader: " << e.what();
            throw;
        }
    }

    void WorldLoader::_LoadMapConf(Game::Map::Conf& conf,
                                   std::string const& name,
                                   std::string const& code,
                                   Game::World const& world)
    {
        conf.name = name;

        Log::load << "Load lua code:\n'''\n" << code << "\n'''\n";
        Tools::Lua::Interpreter lua;
        lua.DoString(code);
        auto fullname = lua.Globals()["fullname"],
             is_default = lua.Globals()["is_default"],
             cubes = lua.Globals()["cubes"],
             equations = lua.Globals()["equations"];

        conf.fullname = fullname.To<std::string>();
        conf.is_default = is_default.To<bool>();

        for (auto it = equations.Begin(), end = equations.End(); it != end; ++it)
        {
            auto eq = it.GetValue();
            auto& eq_conf = conf.equations[it.GetKey().To<std::string>()];
            Log::load << "Loading equation " << it.GetKey().To<std::string>() << "\n";
            eq_conf.function_name = eq["function_name"].To<std::string>();
            for (auto it2 = eq.Begin(), end2 = eq.End(); it2 != end2; ++it2)
            {
                if (it2.GetValue().IsNumber())
                    eq_conf.values[it2.GetKey().To<std::string>()] = it2.GetValue().To<double>();
            }
        }

        for (auto cubeIt = cubes.Begin(), cubeItEnd = cubes.End(); cubeIt != cubeItEnd; ++cubeIt)
        {
            auto cvb = cubeIt.GetValue(); // Cube validation block, lol
            auto name = cubeIt.GetKey().To<std::string>();
            Log::load << "Loading cube " << cubeIt.GetKey().To<std::string>() << "\n";

            if (conf.cubes.find(name) != conf.cubes.end())
            {
                Log::load << "WARNING: cube \"" << name << "\" already defined, only one definition will be used.\n";
                Tools::error << "WARNING: cube \"" << name << "\" already defined, only one definition will be used.\n";
                continue;
            }

            auto& cube = conf.cubes[name];
            cube.type = WorldLoader::_GetCubeTypeByName(name, world);
            if (cube.type == 0)
            {
                if (name != "void")
                {
                    Log::load << "WARNING: cube \"" << name << "\" is not recognized. It will be ignored. If you want to define empty cubes, name it \"void\"\n";
                    Tools::error << "WARNING: cube \"" << name << "\" is not recognized. It will be ignored. If you want to define empty cubes, name it \"void\"\n";
                    continue;
                }
                cube.type = new Common::CubeType(0, "void");
            }

            // Parcours des ValidationBlocConf
            for (auto it = cvb.Begin(), ite = cvb.End(); it != ite; ++it)
            {
                Game::Map::Conf::ValidationBlocConf validation_bloc;

                validation_bloc.cube_type = cube.type;
                validation_bloc.priority = it.GetValue()["priority"].To<int>();

                auto validators = it.GetValue()["validators"];

                // Parcours des ValidatorConf
                for (auto it2 = validators.Begin(), ite2 = validators.End(); it2 != ite2; ++it2)
                {
                    Game::Map::Conf::ValidatorConf validator;
                    validator.equation = it2.GetValue()["equation"].To<std::string>();
                    validator.validator = it2.GetValue()["validator"].To<std::string>();
                    for (auto it3 = it2.GetValue().Begin(), ite3 = it2.GetValue().End(); it3 != ite3; ++it3)
                        if (it3.GetValue().IsNumber())
                            validator.values[it3.GetKey().To<std::string>()] = it3.GetValue().To<double>();
                    validation_bloc.validators.push_back(validator);
                }
                cube.validation_blocs.push_back(validation_bloc);
            }

        }
    }

    Common::CubeType const* WorldLoader::_GetCubeTypeByName(std::string const& name,
                                                            Game::World const& world)
    {
        auto it = world._cubeTypes.begin(),
             end = world._cubeTypes.end();
        for (; it != end; ++it)
        {
            if ((*it).name == name)
                return &(*it);
        }
        return 0;
    }

}}
