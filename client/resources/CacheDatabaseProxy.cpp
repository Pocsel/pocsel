#include "client/precompiled.hpp"

#include "common/Packet.hpp"
#include "common/Resource.hpp"
#include "tools/database/ConnectionPool.hpp"
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
                                           Uint32 worldVersion) :
        _worldVersion(0),
        _connectionPool(0),
        _cacheVersion(0)
    {
        this->_worldVersion = worldVersion;
        this->_worldName = worldName;
        std::string cacheFile = (cacheDir / (host + "-" + worldIdentifier + "." + Common::CacheFileExt)).string();
        this->_CheckCacheFile(cacheFile);

        this->_connectionPool = new Tools::Database::ConnectionPool<Tools::Database::Sqlite::Connection>(cacheFile);
        auto conn = this->_connectionPool->GetConnection();
        auto& curs = conn->GetCursor();
        if (conn->HasTable("cache"))
        {
            curs.Execute("SELECT version FROM cache");
            if (curs.HasData())
                this->_cacheVersion = curs.FetchOne()[0].GetInt();
            else
                throw std::runtime_error("Corrupted cache");
        }
        else
        {
            curs.Execute("CREATE TABLE cache (version INTEGER, format_version INTEGER, world_name TEXT)");
            curs.Execute("INSERT INTO cache (version, format_version) VALUES (0, ?)").Bind(CacheFormatVersion);
            curs.Execute("CREATE TABLE resource (id INTEGER, type TEXT, data BLOB, plugin_id INTEGER, filename TEXT)");
        }
    }

    CacheDatabaseProxy::~CacheDatabaseProxy()
    {
        Tools::Delete(this->_connectionPool);
    }

    void CacheDatabaseProxy::_CheckCacheFile(std::string const& file)
    {
        if (!boost::filesystem::exists(file))
            return;
        try
        {
            Tools::Database::ConnectionPool<Tools::Database::Sqlite::Connection> pool(file);
            auto conn = pool.GetConnection();
            auto& curs = conn->GetCursor();
            if (conn->HasTable("cache"))
            {
                curs.Execute("SELECT format_version FROM cache");
                if (curs.HasData())
                {
                    auto& row = curs.FetchOne();
                    if (row[0].GetInt() != CacheFormatVersion)
                        throw std::runtime_error("bad version");
                }
                else
                    throw std::runtime_error("no metadata");
            }
            else
                throw std::runtime_error("no metadata table");
            if (!conn->HasTable("resource"))
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
        if (!this->_connectionPool)
        {
            Tools::error << "CacheDatabaseProxy::AddResource: Adding resource to an unloaded cache.\n";
            return;
        }
        auto conn = this->_connectionPool->GetConnection();
        auto& curs = conn->GetCursor();
        curs.Execute("DELETE FROM resource WHERE id = ?").Bind(res.id);
        Tools::Database::Blob b(res.data, res.size);
        curs.Execute(
            "INSERT INTO resource (id, type, data, plugin_id, filename) "
            "VALUES (?, ?, ?, ?, ?)"
        ).Bind(res.id).Bind(res.type).Bind(b).Bind(res.pluginId).Bind(res.filename);
    }

    Common::Resource* CacheDatabaseProxy::GetResource(Uint32 id)
    {
        try
        {
            auto conn = this->_connectionPool->GetConnection();
            auto& curs = conn->GetCursor();
            curs.Execute("SELECT id, plugin_id, type, filename, data FROM resource WHERE id = ?").Bind(id);
            if (curs.HasData())
            {
                auto& row = curs.FetchOne();
                return new Common::Resource(row[0].GetInt(),
                        row[1].GetInt(),
                        row[2].GetString(),
                        row[3].GetString(),
                        row[4].GetBlob().data,
                        (Uint32)row[4].GetBlob().size);
            }
            else
                return 0;
        }
        catch (std::exception&)
        {
            return 0;
        }
    }

    void CacheDatabaseProxy::ValidateUpdate()
    {
        auto conn = this->_connectionPool->GetConnection();
        auto& curs = conn->GetCursor();
        curs.Execute("UPDATE cache SET version = ?, world_name = ?").Bind(this->_worldVersion).Bind(this->_worldName);
        Tools::log << "Cache validated for version " << this->_worldVersion << " \"" << this->_worldName << "\".\n";
    }

}}
