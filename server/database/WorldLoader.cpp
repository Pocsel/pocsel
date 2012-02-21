#include "server/database/WorldLoader.hpp"
#include "server/database/ResourceManager.hpp"

#include "tools/database/IConnectionPool.hpp"

#include "tools/lua/Interpreter.hpp"

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
        curs.Execute("SELECT identifier, fullname, version FROM world");
        if (curs.HasData())
        {
            auto& row = curs.FetchOne();
            world._identifier = row[0].GetString();
            world._fullname = row[1].GetString();
            world._version = row[2].GetInt();
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
            Game::Map::Conf conf;
            WorldLoader::_LoadMapConf(conf, row[0].GetString(), row[1].GetString(), world);
            conf.cubeTypes = &world._cubeTypes;
            world._maps[conf.name] = new Game::Map::Map(conf, world._messageQueue);
            if (conf.is_default) {
                world._defaultMap = world._maps[conf.name];
            }
        }
        if (world._defaultMap == 0)
            throw std::runtime_error("Cannot find default map");
    }

    void WorldLoader::_LoadCubeType(Common::CubeType& descr, std::string const& code, ResourceManager const& manager)
    {
        Tools::Lua::Interpreter lua(code);
        try
        {
            auto textures = lua["textures"];
            descr.textures.top =    manager.GetId(textures["top"].as<std::string>());
            descr.textures.left =   manager.GetId(textures["left"].as<std::string>());
            descr.textures.front =  manager.GetId(textures["front"].as<std::string>());
            descr.textures.right =  manager.GetId(textures["right"].as<std::string>());
            descr.textures.back =   manager.GetId(textures["back"].as<std::string>());
            descr.textures.bottom = manager.GetId(textures["bottom"].as<std::string>());

            descr.solid = lua["solid"];
            descr.transparent = lua["transparent"];

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
            Tools::error << "CubeTypeLoader::Load: " << e.what() << ".\n";
            throw;
        }
    }

    void WorldLoader::_LoadMapConf(Game::Map::Conf& conf,
                                   std::string const& name,
                                   std::string const& code,
                                   Game::World const& world)
    {
        // TODO rajouter des verifications au niveau de la lecture du lua
        // genre:
        // est-ce que ce truc est bien un int ?
        // est-ce que ca va pas faire un throw ?
        // est-ce que l'utilisateur sait coder ?

        conf.name = name;


        Log::load << "Load lua code:\n'''\n" << code << "\n'''\n";
        Tools::Lua::Interpreter lua(code);
        auto fullname = lua["fullname"],
             is_default = lua["is_default"],
             cubes = lua["cubes"],
             equations = lua["equations"];

        lua.DumpStack(Log::load);
        conf.fullname = fullname.as<std::string>();
        conf.is_default = is_default.as<bool>();

        for (auto it = equations.begin(), end = equations.end(); it != end; ++it)
        {
            auto eq = it.value();
            auto& eq_conf = conf.equations[it.key()];
            Log::load << "Loading equation " << it.key() << "\n";
            eq_conf.function_name = eq["function_name"].as<std::string>();
            for (auto it = eq.begin(), end = eq.end(); it != end; ++it)
            {
                if (it.value().IsNumber())
                    eq_conf.values[it.key().as<std::string>()] = it.value().as<double>();
            }
        }
        for (auto it = cubes.begin(), end = cubes.end(); it != end; ++it)
        {
            auto cf = it.value();
            auto name = it.key().as<std::string>();
            Log::load << "Loading cube " << it.key() << "\n";

            if (conf.cubes.find(name) != conf.cubes.end())
            {
                Log::load <<
                    "WARNING: cube '" << name <<
                    "' already defined, only one definition will be used.\n";
                continue;
            }

            // XXX fait un assert dans la lecture du lua, c'est ptet pas une bonne idée
    //        assert(conf.cubes.find(name) == conf.cubes.end());

            auto& cube = conf.cubes[name];
            cube.type = WorldLoader::_GetCubeTypeByName(name, world);
            if (cube.type == 0)
            {
                if (name != "void")
                {
                    Log::load << "WARNING: cube '" << name <<
                        "' is not recognized. It will be ignored." <<
                        "If you want to define empty cubes, name it 'void'\n";
                    continue;
                }

                cube.type = new Common::CubeType(0, "void");
            }

            // Parcours des ValidationBlocConf
            for (auto it = cf.begin(), ite = cf.end(); it != ite; ++it)
            {
                Game::Map::Conf::ValidationBlocConf validation_bloc;

                validation_bloc.cube_type = cube.type;
                validation_bloc.priority = (*it)["priority"].as<int>();

                auto validators = (*it)["validators"];

                // Parcours des ValidatorConf
                for (auto it2 = validators.begin(), ite2 = validators.end(); it2 != ite2; ++it2)
                {
                    Game::Map::Conf::ValidatorConf validator;
                    validator.equation = (*it2)["equation"].as<std::string>();
                    validator.validator = (*it2)["validator"].as<std::string>();
                    for (auto it3 = (*it2).begin(), ite3 = (*it2).end(); it3 != ite3; ++it3)
                    {
                        if (it3.value().IsNumber())
                        {
                            validator.values[it3.key().as<std::string>()] = it3.value().as<double>();
                        }
                    }
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
