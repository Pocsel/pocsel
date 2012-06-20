#include "server/database/WorldLoader2.hpp"
#include "server/database/ResourceManager.hpp"
#include "tools/database/IConnection.hpp"
#include "common/FieldUtils.hpp"
#include "server/game/World.hpp"
#include "server/game/PluginManager.hpp"
#include "server/game/map/Map.hpp"
#include "server/Logger.hpp"

namespace Server { namespace Database {

    WorldLoader2::WorldLoader2(Game::World& world, ResourceManager& resourceManager) :
        _resourceManager(resourceManager),
        _conn(resourceManager.GetConnection()),
        _world(world)
    {
        this->_LoadMetadata();
        this->_LoadPlugins();
        this->_LoadMaps();
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
                    throw std::runtime_error("WorldLoader2::_LoadMaps(): Invalid map name \"" + name + "\".");
                std::string lua = row->GetString(1);
                Uint64 time = row->GetUint64(2);

                std::vector<Game::Map::Chunk::IdType> existingBigChunksIds;
                auto q = this->_conn.CreateQuery("SELECT id FROM " + name + "_bigchunk");
                while (auto r = q->Fetch())
                    existingBigChunksIds.push_back(r->GetUint64(0));
            }
            catch (std::exception& e)
            {
                Log::load << "WorldLoader: Failed to load map \"" << name << "\": " << e.what() << std::endl;
                Tools::error << "WorldLoader: Failed to load map \"" << name << "\": " << e.what() << std::endl;
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
    }

}}
