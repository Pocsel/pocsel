#include "server/database/WorldLoader2.hpp"
#include "server/database/ResourceManager.hpp"
#include "tools/database/IConnection.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Iterator.hpp"
#include "common/FieldUtils.hpp"
#include "server/game/World.hpp"
#include "server/game/PluginManager.hpp"
#include "server/game/map/Map.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/Game.hpp"
#include "server/game/engine/EntityManager.hpp"
#include "server/Server.hpp"
#include "server/Settings.hpp"
#include "server/rcon/Rcon.hpp"
#include "server/rcon/EntityManager.hpp"
#include "server/Logger.hpp"

namespace Server { namespace Database {

    WorldLoader2::WorldLoader2(Game::World& world, ResourceManager& resourceManager) :
        _resourceManager(resourceManager),
        _conn(resourceManager.GetConnection()),
        _world(world),
        _currentMap(0)
    {
        this->_LoadMetadata();
        this->_LoadPlugins();
        this->_LoadMaps();
        this->_LoadServerFiles();
        this->_LoadCubeTypes();
    }

    WorldLoader2::~WorldLoader2()
    {
        auto it = this->_tmpMaps.begin();
        auto itEnd = this->_tmpMaps.end();
        for (; it != itEnd; ++it)
            Tools::Delete(it->second);
    }

    void WorldLoader2::_LoadMetadata()
    {
        auto query = this->_conn.CreateQuery("SELECT identifier, fullname, version, build_hash FROM world");
        if (auto row = query->Fetch())
        {
            if (!Common::FieldUtils::IsWorldIdentifier(this->_world._identifier = row->GetString(0)))
                throw std::runtime_error("WorldLoader2::_LoadMetadata: Invalid world identifier \"" + this->_world._identifier + "\".");
            if (!Common::FieldUtils::IsWorldFullname(this->_world._fullname = row->GetString(1)))
                throw std::runtime_error("WorldLoader2::_LoadMetadata: Invalid world full name \"" + this->_world._fullname + "\".");
            this->_world._version = row->GetUint32(2);
            if (!Common::FieldUtils::IsWorldBuildHash(this->_world._buildHash = row->GetString(3)))
                throw std::runtime_error("WorldLoader2::_LoadMetadata: Invalid world build hash.");
        }
        else
            throw std::runtime_error("WorldLoader2::_LoadMetadata: World file is missing metadata (invalid or corrupt world file).");
    }

    void WorldLoader2::_LoadPlugins()
    {
        auto query = this->_conn.CreateQuery("SELECT id, fullname, identifier FROM plugin");
        while (auto row = query->Fetch())
            try
            {
                this->_world.GetPluginManager().AddPlugin(row->GetUint32(0), row->GetString(1), row->GetString(2));
            }
            catch (std::exception& e)
            {
                Log::load << "WorldLoader: Failed to load plugin \"" << row->GetString(1) << "\": " << e.what() << std::endl;
                Tools::error << "WorldLoader: Failed to load plugin \"" << row->GetString(1) << "\": " << e.what() << std::endl;
                throw;
            }
    }

    void WorldLoader2::_LoadMaps()
    {
        auto query = this->_conn.CreateQuery("SELECT name, lua, time FROM map");
        while (auto row = query->Fetch())
        {
            std::string name = row->GetString(0);
            try
            {
                if (!Common::FieldUtils::IsMapName(name))
                    throw std::runtime_error("WorldLoader2::_LoadMaps: Invalid map name \"" + name + "\".");
                if (this->_world._maps.find(name) != this->_world._maps.end())
                    throw std::runtime_error("WorldLoader2::_LoadMaps: A map named \"" + name + "\" already exists.");
                std::string lua = row->GetString(1);
                Uint64 time = row->GetUint64(2);

                std::vector<Game::Map::Chunk::IdType> existingBigChunksIds;
                auto q = this->_conn.CreateQuery("SELECT id FROM " + name + "_bigchunk");
                while (auto r = q->Fetch())
                    existingBigChunksIds.push_back(r->GetUint64(0));

                Game::Map::Conf conf;
                this->_ParseMapConf(name, lua, conf);

                auto map = new Game::Map::Map(conf, time, this->_world, existingBigChunksIds);
                this->_world._maps[name] = map;
                if (conf.is_default)
                    this->_world._defaultMap = map;
            }
            catch (std::exception& e)
            {
                Log::load << "WorldLoader: Failed to load map \"" << name << "\": " << e.what() << std::endl;
                Tools::error << "WorldLoader: Failed to load map \"" << name << "\": " << e.what() << std::endl;
            }
        }
    }

    void WorldLoader2::_ParseMapConf(std::string const& name, std::string const& lua, Game::Map::Conf& conf)
    {
        auto interpreter = new Tools::Lua::Interpreter();
        Log::load << "Load lua code:\n'''\n" << lua << "\n'''\n";
        interpreter->DoString(lua);

        // conf de base
        conf.name = name;
        conf.fullname = interpreter->Globals()["fullname"].To<std::string>();
        conf.is_default = interpreter->Globals()["is_default"].To<bool>();

        // parsing des equations
        auto it = interpreter->Globals()["equations"].Begin();
        auto itEnd = interpreter->Globals()["equations"].End();
        for (; it != itEnd; ++it)
        {
            auto eq = it.GetValue();
            auto& eq_conf = conf.equations[it.GetKey().To<std::string>()];
            Log::load << "Loading equation " << it.GetKey().To<std::string>() << "\n";
            eq_conf.function_name = eq["function_name"].To<std::string>();
            auto valuesIt = eq.Begin();
            auto valuesItEnd = eq.End();
            for (; valuesIt != valuesItEnd; ++valuesIt)
                if (valuesIt.GetValue().IsNumber())
                    eq_conf.values[valuesIt.GetKey().To<std::string>()] = valuesIt.GetValue().To<double>();
        }

        // sauve la table cubes pour la parser plus tard (quand les types de cubes seront tous enregistrés)
        this->_tmpMaps.push_back(std::make_pair(name, interpreter));
    }

    void WorldLoader2::_LoadServerFiles()
    {
        auto query = this->_conn.CreateQuery("SELECT plugin_id, name, lua FROM server_file");
        auto itMap = this->_world._maps.begin();
        auto itMapEnd = this->_world._maps.end();
        for (; itMap != itMapEnd; ++itMap)
        {
            this->_currentMap = itMap->second;
            auto& interpreter = itMap->second->GetEngine().GetInterpreter();
            interpreter.Globals().GetTable("Server").GetTable("Cube").Set("Register", interpreter.MakeFunction(std::bind(&WorldLoader2::_ApiRegisterCube, this, std::placeholders::_1)));
            interpreter.Globals().Set("require", interpreter.MakeFunction(std::bind(&WorldLoader2::_ApiRequire, this, std::placeholders::_1)));
            while (auto row = query->Fetch())
                try
                {
                    this->_LoadServerFile(row->GetUint32(0), row->GetString(1));
                }
                catch (std::exception& e)
                {
                    Log::load << "WorldLoader2::_LoadServerFiles: Failed to load server file \"" << row->GetString(1) << "\" in map \"" << this->_currentMap->GetName() << "\": " << e.what() << std::endl;
                    Tools::error << "WorldLoader2::_LoadServerFiles: Failed to load server file \"" << row->GetString(1) << "\" in map \"" << this->_currentMap->GetName() << "\": " << e.what() << std::endl;
                }
            this->_currentMap->GetEngine().OverrideRunningPluginId(0);
            this->_modules.clear();
            interpreter.Globals().GetTable("Server").Set("Cube", interpreter.MakeNil());
            interpreter.Globals().Set("require", interpreter.MakeNil());
            query->Reset();
        }
        this->_currentMap = 0;

        // enregistre la liste des fichiers du serveur pour rcon
        if (this->_world.GetGame().GetServer().GetSettings().debug)
            while (auto row = query->Fetch())
                this->_world.GetGame().GetServer().GetRcon().GetEntityManager().AddFile(row->GetUint32(0), row->GetString(1), row->GetString(2));

        // enregistre les types d'entités pour rcon
        if (!this->_world._maps.empty())
            this->_world._maps.begin()->second->GetEngine().GetEntityManager().RconAddEntityTypes(this->_world.GetGame().GetServer().GetRcon().GetEntityManager());
    }

    Tools::Lua::Ref WorldLoader2::_LoadServerFile(Uint32 pluginId, std::string const& name)
    {
        // check si le fichier a déjà été chargé
        auto itPlugin = this->_modules.find(pluginId);
        if (itPlugin != this->_modules.end())
        {
            auto it = itPlugin->second.find(name);
            if (it != itPlugin->second.end())
            {
                if (it->second.first) // loading in progress
                {
                    Log::load << "WorldLoader2::_LoadServerFile: Warning: Recursive require() of \"" << name << "\" in plugin " << pluginId << " in map \"" << this->_currentMap->GetName() << "\" (returning nil)." << std::endl;
                    Tools::error << "WorldLoader2::_LoadServerFile: Warning: Recursive require() of \"" << name << "\" in plugin " << pluginId << " in map \"" << this->_currentMap->GetName() << "\" (returning nil)." << std::endl;
                }
                return it->second.second;
            }
        }

        // le fichier n'est pas déjà chargé :
        auto query = this->_conn.CreateQuery("SELECT lua FROM server_file WHERE plugin_id = ? AND name = ?");
        query->Bind(pluginId).Bind(name);
        if (auto row = query->Fetch())
        {
            // ajoute un bool dans _modules pour eviter une récursion infinie
            auto it = this->_modules[pluginId].insert(std::make_pair(name, std::make_pair(true /* loading in progress */, this->_currentMap->GetEngine().GetInterpreter().MakeNil())));

            // on change de fichier, donc potentiellement de runningPluginId
            Uint32 previousRunningPluginId = this->_currentMap->GetEngine().GetRunningPluginId();
            this->_currentMap->GetEngine().OverrideRunningPluginId(pluginId);

            // chargement du lua
            Tools::Lua::Ref f = this->_currentMap->GetEngine().GetInterpreter().MakeNil();
            Tools::Lua::Ref module = this->_currentMap->GetEngine().GetInterpreter().MakeNil();
            try
            {
                auto f = this->_currentMap->GetEngine().GetInterpreter().LoadString(row->GetString(0));
                auto module = f();
            }
            catch (std::exception& e)
            {
                this->_currentMap->GetEngine().OverrideRunningPluginId(previousRunningPluginId); // chargement foiré, on revien quand meme dans le plugin précédent
                it.first->second.first = false; // loading not in progress anymore
                throw;
            }
            if (module == f) // le module se retourne lui-même = pas de return dans le module = on retourne nil
                module = this->_currentMap->GetEngine().GetInterpreter().MakeNil();

            // chargement fini, on revien dans le fichier qui a appelé require()
            this->_currentMap->GetEngine().OverrideRunningPluginId(previousRunningPluginId);

            // enregistrement du module chargé (évite de charger plusieurs fois le meme fichier)
            it.first->second.second = module;
            it.first->second.first = false; // loading not in progress anymore

            return module;
        }
        throw std::runtime_error("WorldLoader2::_LoadServerFile: Server file \"" + name + "\" in plugin " + Tools::ToString(pluginId) + " not found in map \"" + this->_currentMap->GetName() + "\"");
    }

    void WorldLoader2::_ApiRequire(Tools::Lua::CallHelper& helper)
    {
        std::string name = helper.PopArg("require: Missing argument \"name\"").CheckString("require: Argument \"name\" must be a string");
        Uint32 pluginId = this->_world.GetPluginManager().GetPluginId(Common::FieldUtils::GetPluginNameFromResource(name));
        std::string fileName = Common::FieldUtils::GetResourceNameFromResource(name);
        helper.PushRet(this->_LoadServerFile(pluginId, fileName));
    }

    void WorldLoader2::_ApiRegisterCube(Tools::Lua::CallHelper& helper)
    {
        Tools::Lua::Ref prototype = helper.PopArg();
        std::string name = prototype["cubeName"].Check<std::string>("Server.Cube.Register: Field \"cubeName\" must exist and be a string");
        auto id = this->_GetCubeTypeIdByName(this->_currentMap->GetEngine().GetRunningPluginId(), name);

        Game::Map::CubeType desc(
                id,
                name,
                this->_currentMap->GetEngine().GetRunningPluginId(),
                prototype["material"].Check<std::string>("Server.Cube.Register: Field \"material\" must exist be a string"),
                prototype["solid"].To<bool>(),
                prototype,
                prototype["transparent"].To<bool>());
        this->_currentMap->GetConfiguration().cubeTypes.push_back(desc);

        Log::load << "[map: " << this->_currentMap->GetName() << "] " <<
            "Load cube type " << id <<
            ", plugin_id: " << this->_currentMap->GetEngine().GetRunningPluginId() <<
            ", name: " << name << "\n";
    }

    void WorldLoader2::_LoadCubeTypes()
    {
        auto itTmpMap = this->_tmpMaps.begin();
        auto itTmpMapEnd = this->_tmpMaps.end();
        for (; itTmpMap != itTmpMapEnd; ++itTmpMap)
        {
            auto& map = this->_world.GetMap(itTmpMap->first);
            auto cubes = itTmpMap->second->Globals()["cubes"];
            auto& conf = map.GetConfiguration();
            for (auto cubeIt = cubes.Begin(), cubeItEnd = cubes.End(); cubeIt != cubeItEnd; ++cubeIt)
            {
                auto cvb = cubeIt.GetValue(); // Cube validation block, lol
                auto name = cubeIt.GetKey().To<std::string>();
                Log::load << "Loading cube " << name << "\n";

                if (conf.cubes.find(name) != conf.cubes.end())
                {
                    Log::load << "WARNING: cube \"" << name << "\" already defined in map \"" << map.GetName() << "\", only one definition will be used." << std::endl;
                    Tools::error << "WARNING: cube \"" << name << "\" already defined in map \"" << map.GetName() << "\", only one definition will be used." << std::endl;
                    continue;
                }

                auto& cube = conf.cubes[name];
                cube.type = this->_GetCubeTypeByName(this->_world.GetPluginManager().GetPluginId(Common::FieldUtils::GetPluginNameFromResource(name)),
                        Common::FieldUtils::GetResourceNameFromResource(name));
                if (cube.type == 0)
                {
                    if (name != "Void")
                    {
                        Log::load << "WARNING: cube \"" << name << "\" is not recognized \"" << map.GetName() << "\". It will be ignored. If you want to define empty cubes, name it \"Void\"." << std::endl;
                        Tools::error << "WARNING: cube \"" << name << "\" is not recognized in map \"" << map.GetName() << "\". It will be ignored. If you want to define empty cubes, name it \"Void\"." << std::endl;
                        conf.cubes.erase(name);
                        continue;
                    }
                    cube.type = new Game::Map::CubeType(0, "Void", 0, "Void", false, map.GetEngine().GetInterpreter().MakeTable(), true);
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

    Common::BaseChunk::CubeType WorldLoader2::_GetCubeTypeIdByName(Uint32 pluginId, std::string const& name)
    {
        auto query = this->_conn.CreateQuery("SELECT id FROM cube_id WHERE plugin_id = ? AND name = ?;");
        query->Bind(pluginId).Bind(name);
        static_assert(sizeof(Common::BaseChunk::CubeType) == sizeof(Uint16), "cette fonction assume un maximum de ~65000 types de cubes");
        if (auto row = query->Fetch())
            return row->GetUint16(0);
        else
        {
            this->_conn.CreateQuery("INSERT INTO cube_id (plugin_id, name) VALUES (?, ?);")->Bind(pluginId).Bind(name).ExecuteNonSelect();
            auto id = this->_conn.GetLastInsertedId();
            if (id >= 256 * 256) // XXX
                throw std::runtime_error("too many cube types to load, you should uninstall a plugin");
            return static_cast<Uint16>(id);
        }
    }

    Game::Map::CubeType const* WorldLoader2::_GetCubeTypeByName(Uint32 pluginId, std::string const& name)
    {
        auto it = this->_world.GetDefaultMap().GetCubeTypes().begin();
        auto itEnd = this->_world.GetDefaultMap().GetCubeTypes().end();
        for (; it != itEnd; ++it)
            if (it->name == name && it->pluginId == pluginId)
                return &(*it);
        return 0;
    }

}}
