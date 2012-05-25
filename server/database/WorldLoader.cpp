#include "server/precompiled.hpp"

#include "server/database/WorldLoader.hpp"
#include "server/database/ResourceManager.hpp"

#include "tools/database/IConnection.hpp"

#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Iterator.hpp"

#include "common/Resource.hpp"

#include "server/Server.hpp"
#include "server/Settings.hpp"
#include "server/rcon/Rcon.hpp"
#include "server/rcon/EntityManager.hpp"
#include "server/game/Game.hpp"
#include "server/game/World.hpp"
#include "server/game/PluginManager.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/engine/EntityManager.hpp"
#include "server/game/map/Conf.hpp"
#include "server/game/map/CubeType.hpp"
#include "server/game/map/Map.hpp"

#include "server/Logger.hpp"

namespace Server { namespace Database {

    WorldLoader::LoadingMapConf::LoadingMapConf(
        Tools::Lua::Interpreter* interpreter,
        Tools::Lua::Ref fullname,
        Tools::Lua::Ref isDefault,
        Tools::Lua::Ref cubes,
        Tools::Lua::Ref equations)
        : interpreter(interpreter),
        fullname(fullname),
        isDefault(isDefault),
        cubes(cubes),
        equations(equations)
    {
    }

    WorldLoader::LoadingMapConf::LoadingMapConf(LoadingMapConf&& rhs)
        : interpreter(rhs.interpreter),
        fullname(rhs.fullname),
        isDefault(rhs.isDefault),
        cubes(rhs.cubes),
        equations(rhs.equations)
    {
        rhs.interpreter = 0;
    }

    WorldLoader::LoadingMapConf::~LoadingMapConf()
    {
        this->fullname.Unref();
        this->isDefault.Unref();
        this->cubes.Unref();
        this->equations.Unref();
        Tools::Delete(this->interpreter);
    }

    WorldLoader::WorldLoader(Game::World& world, ResourceManager& manager)
        : _world(world),
        _resourceMgr(manager),
        _connection(manager.GetConnection())
    {
        // Meta data
        auto query = this->_connection.CreateQuery("SELECT identifier, fullname, version, build_hash FROM world");
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
        query = this->_connection.CreateQuery("SELECT name, lua, time FROM map");
        while (auto row = query->Fetch())
        {
            std::string name = row->GetString(0);
            std::string lua = row->GetString(1);
            Uint64 time = row->GetUint64(2);
            try
            {
                std::vector<Game::Map::Chunk::IdType> existingBigChunks;
                auto q = this->_connection.CreateQuery("SELECT id FROM " + name + "_bigchunk");
                while (auto r = q->Fetch())
                    existingBigChunks.push_back(r->GetUint64(0));

                Game::Map::Conf conf;
                auto lmc = WorldLoader::_LoadMapConf(conf, name, lua);
                auto map = new Game::Map::Map(conf, time, world, existingBigChunks);

                if (world._maps.find(conf.name) != world._maps.end())
                    throw std::runtime_error("A map named \"" + conf.name + "\" already exists");
                world._maps[conf.name] = map;

                WorldLoader::_RegisterResourcesFunctions(*map, map->GetEngine().GetInterpreter());
                if (conf.is_default)
                    world._defaultMap = world._maps[conf.name];

                this->_loadingMaps.insert(std::make_pair(map, std::move(lmc)));
                lmc.interpreter = 0;
            }
            catch (std::exception& e)
            {
                Log::load << "WorldLoader: Failed to load map \"" << name << "\": " << e.what() << std::endl;
                Tools::error << "WorldLoader: Failed to load map \"" << name << "\": " << e.what() << std::endl;
            }
        }
        if (world._defaultMap == 0)
            throw std::runtime_error("Cannot find default map");

        // Plugins
        query = this->_connection.CreateQuery("SELECT id, fullname, identifier FROM plugin");
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
            WorldLoader::_LoadCubeTypes(*itMap->second);
        this->_LoadMapCubeTypes();

        // Entity types
        query = this->_connection.CreateQuery("SELECT plugin_id, name, lua FROM entity_file");
        while (auto row = query->Fetch())
        {
            try
            {
                auto itMap = world._maps.begin();
                auto itMapEnd = world._maps.end();
                for (; itMap != itMapEnd; ++itMap)
                {
                    itMap->second->GetEngine().OverrideRunningPluginId(row->GetUint32(0));
                    itMap->second->GetEngine().GetInterpreter().DoString(row->GetString(2));
                    itMap->second->GetEngine().OverrideRunningPluginId(0);
                }
                if (world.GetGame().GetServer().GetSettings().debug)
                    world.GetGame().GetServer().GetRcon().GetEntityManager().AddFile(row->GetUint32(0), row->GetString(1), row->GetString(2));
            }
            catch (std::exception& e)
            {
                Log::load << "WorldLoader: Failed to load entity file \"" << row->GetString(1) << "\": " << e.what() << std::endl;
                Tools::error << "WorldLoader: Failed to load entity file \"" << row->GetString(1) << "\": " << e.what() << std::endl;
            }
        }
        if (!world._maps.empty())
            world._maps.begin()->second->GetEngine().GetEntityManager().RconAddEntityTypes(world.GetGame().GetServer().GetRcon().GetEntityManager());
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
                auto pluginId = map.GetGame().GetWorld().GetPluginManager().GetPluginId(helper.PopArg().Check<std::string>());
                helper.PushRet(lua.Make(map.GetGame().GetServer().GetResourceManager().GetId(pluginId, helper.PopArg().Check<std::string>())));
            }));
    }

    void WorldLoader::_LoadCubeTypes(Game::Map::Map& map)
    {
        Uint32 currentPluginId = 0;
        auto& lua = map.GetEngine().GetInterpreter();
        auto cubeTypeNs = lua.Globals().GetTable("Server").GetTable("CubeType");
        cubeTypeNs.Set("Register", lua.MakeFunction([&](Tools::Lua::CallHelper& helper)
            {
                Tools::Lua::Ref cubeType = helper.PopArg();
                std::string name = cubeType["name"].Check<std::string>("Server.CubeType.Register: Field \"name\" must be a string");
                auto id = this->_GetCubeTypeId(map.GetEngine().GetRunningPluginId(), name);

                Game::Map::CubeType desc(id, name, cubeType);
                desc.solid = cubeType["solid"].To<bool>();
                desc.transparent = cubeType["transparent"].To<bool>();
                desc.visualEffect = cubeType["visualEffect"].Check<Uint32>("Server.CubeType.Register: Field \"visualEffect\" must be a number");
                map.GetConfiguration().cubeTypes.push_back(desc);

                Log::load << "[map: " << map.GetName() << "] " <<
                    "Load cube type " << id <<
                    ", plugin_id: " << currentPluginId <<
                    ", name: " << name << "\n";
            }));

        auto query = this->_connection.CreateQuery("SELECT plugin_id, name, lua FROM cube_file");
        while (auto row = query->Fetch())
        {
            currentPluginId = row->GetUint32(0);
            std::string name = row->GetString(1);
            std::string code = row->GetString(2);

            Log::load << "[map: " << map.GetName() << "] Execute " << name << " from plugin " << currentPluginId << "...\n";
            try
            {
                map.GetEngine().OverrideRunningPluginId(currentPluginId);
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
        map.GetEngine().OverrideRunningPluginId(0);
    }

    Common::BaseChunk::CubeType WorldLoader::_GetCubeTypeId(Uint32 pluginId, std::string const& name)
    {
        auto query = this->_connection.CreateQuery("SELECT id FROM cube_id WHERE plugin_id = ? AND name = ?;");
        query->Bind(pluginId).Bind(name);
        static_assert(sizeof(Common::BaseChunk::CubeType) == sizeof(Uint16), "cette fonction assume un maximum de ~65000 types de cubes");
        if (auto row = query->Fetch())
            return row->GetUint16(0);
        else
        {
            this->_connection.CreateQuery("INSERT INTO cube_id (plugin_id, name) VALUES (?, ?);")->Bind(pluginId).Bind(name).ExecuteNonSelect();
            auto id = this->_connection.GetLastInsertedId();
            if (id >= 256 * 256) // XXX
                throw std::runtime_error("too many types of cubes to load, you should uninstall a plugin");
            return (Uint16)id;
        }
    }

    WorldLoader::LoadingMapConf WorldLoader::_LoadMapConf(Game::Map::Conf& conf, std::string const& name, std::string const& code)
    {
        conf.name = name;

        Log::load << "Load lua code:\n'''\n" << code << "\n'''\n";
        std::unique_ptr<Tools::Lua::Interpreter> lua(new Tools::Lua::Interpreter());
        lua->DoString(code);
        auto fullname = lua->Globals()["fullname"],
             is_default = lua->Globals()["is_default"],
             cubes = lua->Globals()["cubes"],
             equations = lua->Globals()["equations"];

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

        return WorldLoader::LoadingMapConf(lua.release(), fullname, is_default, cubes, equations);
    }

    void WorldLoader::_LoadMapCubeTypes()
    {
        for (auto mapIt = this->_loadingMaps.begin(), mapIte = this->_loadingMaps.end(); mapIt != mapIte; ++mapIt)
        {
            auto cubes = mapIt->second.cubes;
            auto& conf = mapIt->first->GetConfiguration();
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
                cube.type = this->_GetCubeTypeByName(name);
                if (cube.type == 0)
                {
                    if (name != "void")
                    {
                        Log::load << "WARNING: cube \"" << name << "\" is not recognized. It will be ignored. If you want to define empty cubes, name it \"void\"\n";
                        Tools::error << "WARNING: cube \"" << name << "\" is not recognized. It will be ignored. If you want to define empty cubes, name it \"void\"\n";
                        continue;
                    }
                    cube.type = new Game::Map::CubeType(0, "void", mapIt->second.interpreter->MakeTable());
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
    }

    Game::Map::CubeType const* WorldLoader::_GetCubeTypeByName(std::string const& name)
    {
        auto it = this->_world.GetDefaultMap().GetCubeTypes().begin(),
             end = this->_world.GetDefaultMap().GetCubeTypes().end();
        for (; it != end; ++it)
        {
            if ((*it).name == name)
                return &(*it);
        }
        return 0;
    }

}}
