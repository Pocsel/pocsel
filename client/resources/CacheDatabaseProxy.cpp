#include "client/precompiled.hpp"

#include "common/Packet.hpp"
#include "common/Resource.hpp"
#include "tools/database/sqlite/Connection.hpp"

#include "client/resources/CacheDatabaseProxy.hpp"
#include "client/resources/ResourceDownloader.hpp"
#include "client/resources/ResourceManager.hpp"

#include "common/constants.hpp"

namespace Client { namespace Resources {

    CacheDatabaseProxy::CacheDatabaseProxy(boost::filesystem::path const& cacheDir,
                                           std::string const& host,
                                           std::string const& worldIdentifier,
                                           std::string const& worldName,
                                           Uint32 worldVersion,
                                           std::string const& worldBuildHash) :
        _worldVersion(0),
        _connection(0),
        _cacheVersion(0)
    {
        this->_worldVersion = worldVersion;
        this->_worldName = worldName;
        this->_worldBuildHash = worldBuildHash;
        std::string cacheFile = (cacheDir / (host + "-" + worldIdentifier + "." + Common::CacheFileExt)).string();
        this->_CheckCacheFile(cacheFile);

        this->_connection = new Tools::Database::Sqlite::Connection(cacheFile);
        if (this->_connection->HasTable("cache"))
        {
            auto query = this->_connection->CreateQuery("SELECT version FROM cache");
            if (auto ptr = query->Fetch())
                this->_cacheVersion = ptr->GetUint32(0);
            else
                throw std::runtime_error("Corrupted cache");
        }
        else
        {
            this->_connection->CreateQuery("CREATE TABLE cache (version INTEGER, format_version INTEGER, world_name TEXT, world_build_hash TEXT)")->ExecuteNonSelect();
            this->_connection->CreateQuery("INSERT INTO cache (version, format_version) VALUES (0, ?)")->Bind(CacheFormatVersion).ExecuteNonSelect();
            this->_connection->CreateQuery("CREATE TABLE resource (id INTEGER, type TEXT, data BLOB, plugin_id INTEGER, filename TEXT)")->ExecuteNonSelect();
        }
    }

    CacheDatabaseProxy::~CacheDatabaseProxy()
    {
        Tools::Delete(this->_connection);
    }

    void CacheDatabaseProxy::_CheckCacheFile(std::string const& file)
    {
        if (!boost::filesystem::exists(file))
            return;
        try
        {
            Tools::Database::Sqlite::Connection conn(file);
            if (conn.HasTable("cache"))
            {
                auto query = conn.CreateQuery("SELECT format_version, world_build_hash FROM cache");
                if (auto row = query->Fetch())
                {
                    if (row->GetUint32(0) != CacheFormatVersion)
                        throw std::runtime_error("bad version");
                    else if (row->GetString(1) != this->_worldBuildHash)
                        throw std::runtime_error("server rebuilt world");
                }
                else
                    throw std::runtime_error("no metadata");
            }
            else
                throw std::runtime_error("no metadata table");
            if (!conn.HasTable("resource"))
                throw std::runtime_error("no resource table");
        }
        catch (std::exception& e)
        {
            Tools::error << "Cache file error: " << e.what() << ". Deleting cache file.\n";
            boost::filesystem::remove(file);
            return;
        }
        Tools::log << "Cache file OK.\n";
    }

    void CacheDatabaseProxy::AddResource(Common::Resource& res)
    {
        if (this->_connection == 0)
        {
            Tools::error << "CacheDatabaseProxy::AddResource: Adding resource to an unloaded cache.\n";
            return;
        }
        this->_connection->BeginTransaction();
        this->_connection->CreateQuery("DELETE FROM resource WHERE id = ?")->Bind(res.id).ExecuteNonSelect();
        this->_connection->CreateQuery("INSERT INTO resource (id, type, data, plugin_id, filename) VALUES (?, ?, ?, ?, ?)")->
            Bind(res.id).Bind(res.type).Bind(res.data, res.size).Bind(res.pluginId).Bind(res.filename).ExecuteNonSelect();
        this->_connection->EndTransaction();
    }

    std::unique_ptr<Common::Resource> CacheDatabaseProxy::GetResource(Uint32 id)
    {
        try
        {
            auto query = this->_connection->CreateQuery("SELECT id, plugin_id, type, filename, data FROM resource WHERE id = ?");
            query->Bind(id);
            if (auto row = query->Fetch())
            {
                auto arr = row->GetArray(4);
                return std::unique_ptr<Common::Resource>(new Common::Resource(
                        row->GetUint32(0),
                        row->GetUint32(1),
                        row->GetString(2),
                        row->GetString(3),
                        arr.data(),
                        (Uint32)arr.size()));
            }
            else
                return 0;
        }
        catch (std::exception& e)
        {
            Tools::error << "Cache error : " << e.what() << "\n";
            return 0;
        }
    }

    std::list<std::unique_ptr<Common::Resource>> CacheDatabaseProxy::GetAllResources(std::string const& type)
    {
        std::list<std::unique_ptr<Common::Resource>> resources;
        try
        {
            auto query = this->_connection->CreateQuery("SELECT id, plugin_id, type, filename, data FROM resource WHERE type LIKE ?");
            query->Bind(type);
            while (auto row = query->Fetch())
            {
                auto const& arr = row->GetArray(4);
                resources.push_back(std::unique_ptr<Common::Resource>(new Common::Resource(
                        row->GetUint32(0),
                        row->GetUint32(1),
                        row->GetString(2),
                        row->GetString(3),
                        arr.data(),
                        (Uint32)arr.size())));
            }
        }
        catch (std::exception& e)
        {
            Tools::error << "Cache error : " << e.what() << "\n";
        }
        return resources;
    }

    Uint32 CacheDatabaseProxy::GetResourceId(Uint32 pluginId, std::string const& filename)
    {
        try
        {
            auto query = this->_connection->CreateQuery("SELECT id FROM resource WHERE plugin_id = ? AND filename = ?");
            query->Bind(pluginId).Bind(filename);
            if (auto row = query->Fetch())
                return row->GetUint32(0);
            else
                return 0;
        }
        catch (std::exception& e)
        {
            Tools::error << "Cache error : " << e.what() << "\n";
            return 0;
        }
    }

    void CacheDatabaseProxy::ValidateUpdate()
    {
        this->_connection->CreateQuery("UPDATE cache SET version = ?, world_name = ?, world_build_hash = ?")->
            Bind(this->_worldVersion).Bind(this->_worldName).Bind(this->_worldBuildHash).ExecuteNonSelect();
        Tools::log << "Cache validated for version " << this->_worldVersion << " \"" << this->_worldName << "\" (build hash \"" << this->_worldBuildHash << "\").\n";
    }

}}
