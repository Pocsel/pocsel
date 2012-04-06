#include "server/precompiled.hpp"

#include "server/database/WorldLoader.hpp"
#include "server/database/ResourceManager.hpp"

#include "tools/database/IConnection.hpp"

#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Iterator.hpp"

#include "common/Resource.hpp"

#include "server/Server.hpp"
#include "server/game/Game.hpp"
#include "server/game/World.hpp"
#include "server/game/PluginManager.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/engine/EntityManager.hpp"

#include "server/game/map/Conf.hpp"
#include "server/game/map/Map.hpp"

#include "server/Logger.hpp"

namespace Server { namespace Database {

    void WorldLoader::Load(Game::World& world, ResourceManager& manager)
    {
        auto& conn = manager.GetConnection();

        // Meta data
        auto query = conn.CreateQuery("SELECT identifier, fullname, version, build_hash FROM world");
        if (auto row = query->Fetch())
        {
            world._identifier = row->GetString(0);
            world._fullname = row->GetString(1);
            world._version = row->GetUint32(2);
            world._buildHash = row->GetString(3);
        }
        else
            throw std::runtime_error("WorldLoader::Load: World file is missing metadata (invalid or corrupt world file).");

        // Maps
        query = conn.CreateQuery("SELECT name, lua, tick FROM map");
        while (auto row = query->Fetch())
        {
            try
            {
                Game::Map::Conf conf;
                WorldLoader::_LoadMapConf(conf, row->GetString(0), row->GetString(1), world);
                Uint64 curTime = row->GetUint64(2);

                std::vector<Game::Map::Chunk::IdType> existingChunks;
                std::vector<Game::Map::Chunk::IdType> existingBigChunks;

                auto q = conn.CreateQuery("SELECT id FROM " + row->GetString(0) + "_bigchunk");
                while (auto r = q->Fetch())
                    existingBigChunks.push_back(r->GetUint64(0));

                world._maps[conf.name] = new Game::Map::Map(conf, curTime, world._game, existingBigChunks);
                if (conf.is_default)
                    world._defaultMap = world._maps[conf.name];
            }
            catch (std::exception& e)
            {
                Log::load << "WorldLoader: Failed to load map \"" << row->GetString(0) << "\": " << e.what() << std::endl;
                Tools::error << "WorldLoader: Failed to load map \"" << row->GetString(0) << "\": " << e.what() << std::endl;
            }
        }
        if (world._defaultMap == 0)
            throw std::runtime_error("Cannot find default map");

        // Plugins
        query = conn.CreateQuery("SELECT id, fullname, identifier FROM plugin");
        while (auto row = query->Fetch())
        {
            try
            {
                world.GetPluginManager().AddPlugin(row->GetUint32(0), row->GetString(1), row->GetString(2));
            }
            catch (std::exception& e)
            {
                Log::load << "WorldLoader: Failed to load plugin \"" << row->GetString(1) << "\": " << e.what() << std::endl;
                Tools::error << "WorldLoader: Failed to load plugin \"" << row->GetString(1) << "\": " << e.what() << std::endl;
            }
        }

        // CubeTypes
        for (auto itMap = world._maps.begin(), itMapEnd = world._maps.end(); itMap != itMapEnd; ++itMap)
            WorldLoader::_LoadCubeTypes(conn, *itMap->second, manager);

        // Entity types
        query = conn.CreateQuery("SELECT plugin_id, name, lua FROM entity_file");
        while (auto row = query->Fetch())
        {
            try
            {
                auto itMap = world._maps.begin();
                auto itMapEnd = world._maps.end();
                for (; itMap != itMapEnd; ++itMap)
                {
                    itMap->second->GetEngine().GetEntityManager().BeginPluginRegistering(row->GetUint32(0), row->GetString(1));
                    itMap->second->GetEngine().GetInterpreter().DoString(row->GetString(2));
                    itMap->second->GetEngine().GetEntityManager().EndPluginRegistering();
                }
            }
            catch (std::exception& e)
            {
                Log::load << "WorldLoader: Failed to load entity file \"" << row->GetString(1) << "\": " << e.what() << std::endl;
                Tools::error << "WorldLoader: Failed to load entity file \"" << row->GetString(1) << "\": " << e.what() << std::endl;
            }
        }
    }

    void WorldLoader::_RegisterResourcesFunctions(Game::Map::Map& map, Tools::Lua::Interpreter& lua)
    {
        auto resourceNs = lua.Globals().GetTable("Server").GetTable("Resource");
        resourceNs.Set("GetEffect", lua.MakeFunction(
            [&](Tools::Lua::CallHelper& helper)
            {
                auto pluginId = map.GetEngine().GetRunningPluginId();
                helper.PushRet(lua.Make(map.GetGame().GetServer().GetResourceManager().GetId(pluginId, helper.PopArg().Check<std::string>())));
            }));
        resourceNs.Set("GetEffectFromPlugin", lua.MakeFunction(
            [&](Tools::Lua::CallHelper& helper)
            {
                // TODO pluginId !
                auto pluginId = map.GetGame().GetWorld().GetPluginManager().GetPluginId(helper.PopArg().Check<std::string>());
                auto pluginId = map.GetEngine().GetRunningPluginId();
                helper.PushRet(lua.Make(map.GetGame().GetServer().GetResourceManager().GetId(pluginId, helper.PopArg().Check<std::string>())));
            }));
    }

    void WorldLoader::_LoadCubeTypes(Tools::Database::IConnection& conn, Game::Map::Map& map, ResourceManager& manager)
    {
        Uint32 currentPluginId = 0;
        auto& lua = map.GetEngine().GetInterpreter();
        auto cubeTypeNs = lua.Globals().GetTable("Server").GetTable("CubeType");
        cubeTypeNs.Set("Register", lua.Bind(
            [&](Tools::Lua::Ref cubeType)
            {
                Common::BaseChunk::CubeType id;
                std::string name = cubeType["name"].Check<std::string>();
                Uint32 visualEffect = cubeType["visualEffect"].Check<Uint32>();
                Common::CubeType desc(id, name);
                desc.solid = cubeType["solid"].To<bool>();
                desc.transparent = cubeType["transparent"].To<bool>();

                Log::load << "[map: " << map.GetName() << "] " <<
                    "Load cube type " << id <<
                    ", plugin_id: " << currentPluginId <<
                    ", name: " << name << "\n";
            }));

        auto query = conn.CreateQuery("SELECT plugin_id, name, lua FROM cube_type ORDER BY id, plugin_id");
        while (auto row = query->Fetch())
        {
            currentPluginId = row->GetUint64(0);
            std::string name = row->GetString(1);
            std::string code = row->GetString(2);

            Log::load << "[map: " << map.GetName() << "] Execute " << name << ".lua...\n";
            try
            {
                lua.DoString(code);
            }
            catch (std::exception& e)
            {
                Tools::error << "WorldLoader: " << e.what();
                Log::load << "WorldLoader: " << e.what();
                throw;
            }
        }

        cubeTypeNs.Set("Register", lua.MakeNil());
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
