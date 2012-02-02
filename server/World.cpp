
#include <cassert>
#include <iostream>

#include "tools/lua/Interpreter.hpp"

#include "server/CubeTypeLoader.hpp"
#include "server/Map.hpp"
#include "server/ResourceManager.hpp"
#include "server/World.hpp"

using namespace Server;

World::World(Tools::Database::IConnectionPool& pool) :
    _maps(),
    _defaultMap(0),
    _resourceManager(new ResourceManager(pool)),
    _cubeTypeLoader(new CubeTypeLoader(*_resourceManager))
{
    auto conn = pool.GetConnection();
    auto& curs = conn->GetCursor();

    // Meta data
    curs.Execute("SELECT identifier, fullname, version FROM world");
    if (curs.HasData())
    {
        auto& row = curs.FetchOne();
        this->_identifier = row[0].GetString();
        this->_fullname = row[1].GetString();
        this->_version = row[2].GetInt();
    }
    else
        throw std::runtime_error("World file missing metadata.");

    // Cube merde
    curs.Execute("SELECT id, plugin_id, name, lua FROM cube_type");
    while (curs.HasData())
    {
        auto& row = curs.FetchOne();
        Uint32 id = row[0].GetUint64();
        std::string name = row[2].GetString();
        std::cout << "Load cube type id: " << id << ' '
                  << "plugin_id: " << row[1].GetUint64() << ' '
                  << "name: " << row[2].GetString() << std::endl;
        if (this->_cubeTypes.size() < id)
            this->_cubeTypes.resize(id);
        this->_cubeTypes[id - 1].id = id;
        this->_cubeTypes[id - 1].name = name;
        this->_cubeTypeLoader->Load(this->_cubeTypes[id - 1], row[3].GetString());

        std::cout << "Loaded:" << this->_cubeTypes[id-1].solid << "," <<
            this->_cubeTypes[id-1].transparent<<"\n";
    }

    curs.Execute("SELECT name, lua FROM map");
    while (curs.HasData())
    {
        auto& row = curs.FetchOne();
        this->_LoadMap(row[0].GetString(), row[1].GetString());
    }
    if (this->_defaultMap == 0)
        throw std::runtime_error("Cannot find default map");
}

World::~World()
{
    Tools::Delete(this->_cubeTypeLoader);
    Tools::Delete(this->_resourceManager);

    auto it = this->_maps.begin(), end = this->_maps.end();
    for (; it != end; ++it)
    {
        std::cout << "DELETE MAP: " << it->second << "\n";
        delete it->second;
    }
    this->_maps.clear();
}

void World::_LoadMap(std::string const& name, std::string const& code)
{
    // TODO rajouter des verifications au niveau de la lecture du lua
    // genre:
    // est-ce que ce truc est bien un int ?
    // est-ce que ca va pas faire un throw ?
    // est-ce que l'utilisateur sait coder ?

    MapConf conf;
    conf.name = name;


    std::cout << "Load lua code '" << code << "'\n";
    Tools::Lua::Interpreter lua(code);
    auto fullname = lua["fullname"],
         is_default = lua["is_default"],
         cubes = lua["cubes"],
         equations = lua["equations"];

    lua.DumpStack();
    conf.fullname = fullname.as<std::string>();
    conf.is_default = is_default.as<bool>();

    for (auto it = equations.begin(), end = equations.end(); it != end; ++it)
    {
        auto eq = it.value();
        auto& eq_conf = conf.equations[it.key()];
        std::cout << "Loading equation " << it.key() << std::endl;
        eq_conf.function_name = eq["function_name"].as<std::string>();
        for (auto it = eq.begin(), end = eq.end(); it != end; ++it)
        {
            if (it.value().IsNumber())
                eq_conf.values[it.key().as<std::string>()] = it.value().as<double>();
            //XXX OLD
//            else if (it.key().as<std::string>() == "function_name")
//                eq_conf.function_name = it.value().as<std::string>();
        }
    }
    for (auto it = cubes.begin(), end = cubes.end(); it != end; ++it)
    {
        auto cf = it.value();
        auto name = it.key().as<std::string>();
        std::cout << "Loading cube " << it.key() << std::endl;

        if (conf.cubes.find(name) != conf.cubes.end())
        {
            std::cout << "WARNING: cube '" << name <<
                "' already defined, only one definition will be used.\n";
            continue;
        }

        // XXX fait un assert dans la lecture du lua, c'est ptet pas une bonne idée
//        assert(conf.cubes.find(name) == conf.cubes.end());

        auto& cube = conf.cubes[name];
        cube.type = this->_GetCubeTypeByName(name);
        if (cube.type == 0)
        {
            if (name != "void")
            {
                std::cout << "WARNING: cube '" << name <<
                    "' is not recognized. It will be ignored." <<
                    "If you want to define empty cubes, name it 'void'\n";
//                continue;
            }
            // TODO remplacer ce continue par la recuperation de void, ou je sais pas quoi
            continue;
        }

        // Parcours des ValidationBlocConf
        for (auto it = cf.begin(), ite = cf.end(); it != ite; ++it)
        {
            MapConf::ValidationBlocConf validation_bloc;

            validation_bloc.cube_type = cube.type;
            validation_bloc.priority = (*it)["priority"].as<int>();

            auto validators = (*it)["validators"];

            // Parcours des ValidatorConf
            for (auto it2 = validators.begin(), ite2 = validators.end(); it2 != ite2; ++it2)
            {
                MapConf::ValidatorConf validator;
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
        // XXX fait un assert dans la lecture du lua, c'est ptet pas une bonne idée
//        assert(cube.type != 0);
    // XXX OLD
//        cube.priority = cf["priority"].as<unsigned int>();
//        cube.equation = cf["equation"].as<std::string>();
//        auto v = cf["validator"];
//        cube.validator = v["name"].as<std::string>();
//        for (auto it = v.begin(), end = v.end(); it != end; ++it)
//        {
//            if (it.value().IsNumber())
//            {
//                cube.validator_values[it.key().as<std::string>()] = it.value().as<double>();
//            }
//        }
//    }
    conf.cubeTypes = &this->_cubeTypes;
    this->_maps[name] = new Map(conf);
    if (conf.is_default) {
        this->_defaultMap = this->_maps[name];
    }
}

Common::CubeType* World::_GetCubeTypeByName(std::string const& name)
{
    auto it = this->_cubeTypes.begin(),
         end = this->_cubeTypes.end();
    for (; it != end; ++it)
    {
        if ((*it).name == name)
            return &(*it);
    }
    return 0;
}

//size_t World::Poll()
//{
//    assert(this->_map != 0);
//    return this->_map->Poll();
//}
