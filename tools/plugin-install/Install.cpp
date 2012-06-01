#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "tools/plugin-install/Install.hpp"
#include "tools/database/sqlite/Connection.hpp"
#include "common/constants.hpp"
#include "common/FieldValidator.hpp"

namespace Tools { namespace PluginInstall {

    namespace {

        void CreateWorldTables(Tools::Database::IConnection& wconn)
        {
            wconn.CreateQuery("CREATE TABLE cube_file (plugin_id INTEGER, name TEXT, lua TEXT);")->ExecuteNonSelect();
            wconn.CreateQuery("CREATE TABLE cube_id (id INTEGER PRIMARY KEY, plugin_id INTEGER, name TEXT);")->ExecuteNonSelect();
            wconn.CreateQuery("CREATE TABLE entity_file (plugin_id INTEGER, name TEXT, lua TEXT);")->ExecuteNonSelect();
            wconn.CreateQuery("CREATE TABLE map (name TEXT, plugin_id INTEGER, lua TEXT, time INTEGER);")->ExecuteNonSelect();
            wconn.CreateQuery("CREATE TABLE plugin (id INTEGER PRIMARY KEY, build_hash TEXT, fullname TEXT, identifier TEXT);")->ExecuteNonSelect();
            wconn.CreateQuery("CREATE TABLE resource (id INTEGER PRIMARY KEY, plugin_id INTEGER, version INTEGER, name TEXT, type TEXT, data_hash TEXT, data BLOB);")->ExecuteNonSelect();
            wconn.CreateQuery("CREATE TABLE world (version INTEGER, fullname TEXT, format_version INTEGER, identifier TEXT, build_hash TEXT);")->ExecuteNonSelect();
        }

        void FillTableWorld(std::string const& identifier, std::string const& fullname, Tools::Database::IConnection& wconn)
        {
            boost::uuids::random_generator buildHashGenerator;
            // version mise à 0 car elle sera update après à 1
            wconn.CreateQuery("INSERT INTO world (build_hash, fullname, identifier, version, format_version) VALUES (?, ?, ?, ?, ?);")->
                Bind(boost::uuids::to_string(buildHashGenerator())).Bind(fullname).Bind(identifier).Bind(0).Bind(Common::WorldFormatVersion).ExecuteNonSelect();
        }

        Uint32 UpdateTablePlugin(std::string const& pluginBuildHash, std::string const& pluginIdentifier, std::string const& pluginFullname, Tools::Database::IConnection& wconn)
        {
            auto query = wconn.CreateQuery("SELECT id, build_hash FROM plugin WHERE identifier = ?;");
            query->Bind(pluginIdentifier);
            if (auto row = query->Fetch())
            {
                if (row->GetString(1) == pluginBuildHash)
                    throw std::runtime_error("This version of the plugin is already installed in this world.");
                Tools::log << "Different version already installed, updating plugin table." << std::endl;
                wconn.CreateQuery("UPDATE plugin SET build_hash = ?, fullname = ? WHERE id = ?;")->
                    Bind(pluginBuildHash).Bind(pluginFullname).Bind(row->GetUint32(0)).ExecuteNonSelect();
                return row->GetUint32(0);
            }
            Tools::log << "No previous version installed, inserting into plugin table." << std::endl;
            wconn.CreateQuery("INSERT INTO plugin (build_hash, fullname, identifier) VALUES (?, ?, ?);")->
                Bind(pluginBuildHash).Bind(pluginFullname).Bind(pluginIdentifier).ExecuteNonSelect();
            return (Uint32)wconn.GetLastInsertedId();
        }

        void UpdateTableCubeFile(Uint32 pluginId, Tools::Database::IConnection& wconn, Tools::Database::IConnection& pconn)
        {
            wconn.CreateQuery("DELETE FROM cube_file WHERE plugin_id = ?;")->Bind(pluginId).ExecuteNonSelect();
            auto query = pconn.CreateQuery("SELECT name, lua FROM cube_file;");
            while (auto row = query->Fetch())
            {
                wconn.CreateQuery("INSERT INTO cube_file (plugin_id, name, lua) VALUES (?, ?, ?);")->
                    Bind(pluginId).Bind(row->GetString(0)).Bind(row->GetString(1)).ExecuteNonSelect();
                Tools::log << "Added cube file \"" << row->GetString(0) << "\" (size: " << row->GetString(1).size() << " bytes)." << std::endl;
            }
        }

        void UpdateTableEntityFile(Uint32 pluginId, Tools::Database::IConnection& wconn, Tools::Database::IConnection& pconn)
        {
            wconn.CreateQuery("DELETE FROM entity_file WHERE plugin_id = ?;")->Bind(pluginId).ExecuteNonSelect();
            auto query = pconn.CreateQuery("SELECT name, lua FROM entity_file;");
            while (auto row = query->Fetch())
            {
                wconn.CreateQuery("INSERT INTO entity_file (plugin_id, name, lua) VALUES (?, ?, ?);")->
                    Bind(pluginId).Bind(row->GetString(0)).Bind(row->GetString(1)).ExecuteNonSelect();
                Tools::log << "Added entity file \"" << row->GetString(0) << "\" (size: " << row->GetString(1).size() << " bytes)." << std::endl;
            }
        }

        void UpdateTableResource(Uint32 pluginId, Uint32 newWorldVersion, Tools::Database::IConnection& wconn, Tools::Database::IConnection& pconn)
        {
            // liste des id de resource avant toute modif
            std::list<Uint32> previousIds;
            auto previousIdsQuery = wconn.CreateQuery("SELECT id FROM resource WHERE plugin_id = ?;");
            previousIdsQuery->Bind(pluginId);
            while (auto row = previousIdsQuery->Fetch())
                previousIds.push_back(row->GetUint32(0));

            // insertion/maj des resources
            std::list<Uint32> newIds;
            auto pluginQuery = pconn.CreateQuery("SELECT name, type, data_hash, data FROM resource;");
            while (auto pluginRow = pluginQuery->Fetch())
            {
                std::string name = pluginRow->GetString(0);
                std::string type = pluginRow->GetString(1);
                std::string hashPlugin = pluginRow->GetString(2);
                std::vector<char> data = pluginRow->GetArray(3);

                auto worldQuery = wconn.CreateQuery("SELECT id, data_hash FROM resource WHERE plugin_id = ? AND name = ?;");
                worldQuery->Bind(pluginId).Bind(pluginRow->GetString(0));
                if (auto worldRow = worldQuery->Fetch())
                {
                    Uint32 id = worldRow->GetUint32(0);
                    std::string hashWorld = worldRow->GetString(1);

                    if (hashPlugin != hashWorld)
                    {
                        wconn.CreateQuery("UPDATE resource SET version = ?, type = ?, data_hash = ?, data = ? WHERE plugin_id = ? AND name = ?;")->
                            Bind(newWorldVersion).Bind(type).Bind(hashPlugin).Bind(data).Bind(pluginId).Bind(name).ExecuteNonSelect();
                        Tools::log << "Updated resource \"" << name << "\" (size: " << data.size() << " bytes, type: \"" << type << "\", hash: \"" << hashPlugin << "\") to version " << newWorldVersion << "." << std::endl;
                    }
                    else
                        Tools::log << "Resource \"" << name << "\" (size: " << data.size() << " bytes, type: \"" << type << "\", hash: \"" << hashPlugin << "\") not updated." << std::endl;
                    newIds.push_back(id);
                }
                else
                {
                    wconn.CreateQuery("INSERT INTO resource (plugin_id, version, name, type, data_hash, data) VALUES (?, ?, ?, ?, ?, ?);")->
                        Bind(pluginId).Bind(1).Bind(name).Bind(type).Bind(hashPlugin).Bind(data).ExecuteNonSelect();
                    Tools::log << "Added resource \"" << name << "\" (size: " << data.size() << " bytes, type: \"" << type << "\", hash: \"" << hashPlugin << "\")." << std::endl;
                }
            }

            // suppression des resources qui ne sont plus présentes dans le plugin
            auto it = previousIds.begin();
            auto itEnd = previousIds.end();
            for (; it != itEnd; ++it)
            {
                bool found = false;
                auto it2 = newIds.begin();
                auto it2End = newIds.end();
                for (; it2 != it2End; ++it2)
                    if (*it2 == *it)
                    {
                        found = true;
                        break;
                    }
                if (!found)
                {
                    wconn.CreateQuery("DELETE FROM resource WHERE id = ?;")->
                        Bind(*it).ExecuteNonSelect();
                    Tools::log << "Deleted resource " << *it << " (not used)." << std::endl;
                }
            }
        }

        void CreateMap(Uint32 pluginId, std::string const& name, std::string const& lua, Tools::Database::IConnection& wconn)
        {
            wconn.CreateQuery("CREATE TABLE " + name + "_bigchunk (id INTEGER PRIMARY KEY, data BLOB);")->ExecuteNonSelect();
            wconn.CreateQuery("CREATE TABLE " + name + "_callback (id INTEGER, entity_id INTEGER, function TEXT, arg TEXT);")->ExecuteNonSelect();
            wconn.CreateQuery("CREATE TABLE " + name + "_message (time INTEGER, callback_id INTEGER, notification_callback_id INTEGER);")->ExecuteNonSelect();
            wconn.CreateQuery("CREATE TABLE " + name + "_spawn_event (id INTEGER, plugin_id INTEGER, entity_name TEXT, arg TEXT, spawner_id INTEGER, notification_callback_id INTEGER, has_position INTEGER, pos_x REAL, pos_y REAL, pos_z REAL);")->ExecuteNonSelect();
            wconn.CreateQuery("CREATE TABLE " + name + "_kill_event (id INTEGER, entity_id INTEGER, arg TEXT, killer_id INTEGER, notification_callback_id INTEGER);")->ExecuteNonSelect();
            wconn.CreateQuery("CREATE TABLE " + name + "_entity (id INTEGER, plugin_id INTEGER, entity_name TEXT, disabled INTEGER, storage TEXT, pos_x REAL, pos_y REAL, pos_z REAL);")->ExecuteNonSelect();
            wconn.CreateQuery("CREATE TABLE " + name + "_doodad (id INTEGER, plugin_id INTEGER, doodad_name TEXT, entity_id INTEGER, storage TEXT);")->ExecuteNonSelect();
            wconn.CreateQuery("CREATE TABLE " + name + "_initialized_plugin (id INTEGER);")->ExecuteNonSelect();

            wconn.CreateQuery("INSERT INTO map (name, plugin_id, lua, time) VALUES (?, ?, ?, ?);")->
                Bind(name).Bind(pluginId).Bind(lua).Bind(0).ExecuteNonSelect();

            Tools::log << "Added map \"" << name << "\" (size: " << lua.size() << ")." << std::endl;
        }

        void UpdateTableMap(Uint32 pluginId, std::string const& pluginIdentifier, Tools::Database::IConnection& wconn, Tools::Database::IConnection& pconn)
        {
            auto pluginQuery = pconn.CreateQuery("SELECT name, lua FROM map;");
            while (auto pluginRow = pluginQuery->Fetch())
            {
                std::string name = pluginIdentifier + "_" + pluginRow->GetString(0);
                if (!Common::FieldValidator::IsMapName(name))
                    throw std::runtime_error("Invalid map name \"" + name + "\".");
                std::string lua = pluginRow->GetString(1);

                auto worldQuery = wconn.CreateQuery("SELECT 1 FROM map WHERE name = ? AND plugin_id = ?;");
                worldQuery->Bind(name).Bind(pluginId);
                if (worldQuery->Fetch())
                {
                    wconn.CreateQuery("UPDATE map SET lua = ? WHERE name = ? AND plugin_id = ?;")->Bind(lua).Bind(name).Bind(pluginId).ExecuteNonSelect();
                    Tools::log << "Updated map \"" << name << "\" (size: " << lua.size() << ")." << std::endl;
                }
                else
                    CreateMap(pluginId, name, lua, wconn);
            }
        }

        void UpdateTableWorld(Uint32 worldVersion, Tools::Database::IConnection& wconn, Tools::Database::IConnection&)
        {
            wconn.CreateQuery("UPDATE world SET version = ?;")->Bind(worldVersion).ExecuteNonSelect();
        }

    }

    bool Install(boost::filesystem::path const& pluginFile, boost::filesystem::path const& worldFile, std::string const& identifier /* = "" */, std::string const& fullname /* = "" */)
    {
        // check de base des chemins
        if (!boost::filesystem::is_regular_file(pluginFile))
        {
            Tools::log << "Error: " << pluginFile << " is not a regular file." << std::endl;
            return false;
        }
        if (boost::filesystem::exists(worldFile) && !boost::filesystem::is_regular_file(worldFile))
        {
            Tools::log << "Error: " << worldFile << " is not a regular file." << std::endl;
            return false;
        }

        // ouverture plugin
        std::string pluginIdentifier;
        std::string pluginFullname;
        std::string pluginBuildHash;
        Tools::Database::IConnection* pconn = 0;
        try
        {
            pconn = new Tools::Database::Sqlite::Connection(pluginFile.string());
            auto query = pconn->CreateQuery("SELECT build_hash, fullname, identifier, format_version FROM plugin;");
            if (auto row = query->Fetch())
            {
                if (row->GetUint32(3) != Common::PluginFormatVersion)
                    throw std::runtime_error("Bad format version (got " + Tools::ToString(row->GetUint32(3)) + ", expected " + Tools::ToString(Common::PluginFormatVersion) + ").");
                pluginBuildHash = row->GetString(0);
                if (!Common::FieldValidator::IsPluginBuildHash(pluginBuildHash))
                    throw std::runtime_error("Invalid build hash \"" + pluginBuildHash + "\".");
                pluginFullname = row->GetString(1);
                if (!Common::FieldValidator::IsPluginFullname(pluginFullname))
                    throw std::runtime_error("Invalid plugin fullname.");
                pluginIdentifier = row->GetString(2);
                if (!Common::FieldValidator::IsPluginIdentifier(pluginIdentifier))
                    throw std::runtime_error("Invalid plugin identifier.");
            }
            else
                throw std::runtime_error("No metatada.");
        }
        catch (std::exception& e)
        {
            Tools::log << "Error when opening plugin: " << e.what() << std::endl;
            Tools::Delete(pconn);
            return false;
        }

        // ouverture/creation world
        std::string worldIdentifier;
        std::string worldFullname;
        Uint32 newWorldVersion;
        Tools::Database::IConnection* wconn = 0;
        try
        {
            wconn = new Tools::Database::Sqlite::Connection(worldFile.string());
            if (!wconn->HasTable("world"))
            {
                if (!identifier.empty() && !fullname.empty())
                {
                    if (!Common::FieldValidator::IsWorldIdentifier(identifier))
                        throw std::runtime_error("Invalid world identifier \"" + identifier + "\" (must be alphanumeric, 20 characters max.).");
                    if (!Common::FieldValidator::IsWorldFullname(fullname))
                        throw std::runtime_error("Invalid world fullname \"" + fullname + "\" (100 characters max.).");
                    CreateWorldTables(*wconn);
                    FillTableWorld(identifier, fullname, *wconn);
                }
                else
                    throw std::runtime_error("No identifier/fullname specified.");
            }
            auto query = wconn->CreateQuery("SELECT format_version, identifier, fullname, version FROM world;");
            if (auto row = query->Fetch())
            {
                if (row->GetUint32(0) != Common::WorldFormatVersion)
                    throw std::runtime_error("Bad format version (got " + Tools::ToString(row->GetUint32(0)) + ", expected " + Tools::ToString(Common::WorldFormatVersion) + ").");
                worldIdentifier = row->GetString(1);
                if (!Common::FieldValidator::IsWorldIdentifier(worldIdentifier))
                    throw std::runtime_error("Invalid world identifier.");
                worldFullname = row->GetString(2);
                if (!Common::FieldValidator::IsWorldFullname(worldFullname))
                    throw std::runtime_error("Invalid world fullname.");
                newWorldVersion = row->GetUint32(3) + 1;
            }
            else
                throw std::runtime_error("No metadata.");
        }
        catch (std::exception& e)
        {
            Tools::log << "Error when opening/creating world: " << e.what() << std::endl;
            Tools::Delete(wconn);
            Tools::Delete(pconn);
            return false;
        }

        // installation
        Tools::log << "Installing plugin " << pluginFile << " (\"" << pluginIdentifier << "\", \"" << pluginFullname << "\") in world " << worldFile << " (\"" << worldIdentifier << "\", \"" << worldFullname << "\")." << std::endl;
        try
        {
            wconn->BeginTransaction();

            Uint32 pluginId = UpdateTablePlugin(pluginBuildHash, pluginIdentifier, pluginFullname, *wconn);
            UpdateTableCubeFile(pluginId, *wconn, *pconn);
            UpdateTableEntityFile(pluginId, *wconn, *pconn);
            UpdateTableResource(pluginId, newWorldVersion, *wconn, *pconn);
            UpdateTableMap(pluginId, pluginIdentifier, *wconn, *pconn);
            UpdateTableWorld(newWorldVersion, *wconn, *pconn);

            Tools::log << "Finishing writing to database..." << std::endl;
            wconn->EndTransaction();
            Tools::Delete(pconn);
            Tools::Delete(wconn);
        }
        catch (std::exception& e)
        {
            Tools::log << "Error when installing: " << e.what() << std::endl;
            Tools::Delete(pconn);
            Tools::Delete(wconn);
            return false;
        }

        // success!
        Tools::log << "Plugin " << pluginFile << " (\"" << pluginIdentifier << "\", \"" << pluginFullname << "\") successfully installed in world " << worldFile << " (\"" << worldIdentifier << "\", \"" << worldFullname << "\")." << std::endl;
        return true;
    }

    bool Uninstall(std::string const& pluginIdentifier, boost::filesystem::path const& worldFile)
    {
        // TODO or not TODO ?
        return false;
    }

}}
