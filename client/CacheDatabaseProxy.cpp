#include <iostream>
#include <boost/filesystem.hpp>

#include "common/Packet.hpp"
#include "common/Resource.hpp"
#include "tools/database/ConnectionPool.hpp"
#include "tools/database/sqlite/Connection.hpp"

#include "resources/resources.hpp"

#include "client/Network.hpp"
#include "client/PacketCreator.hpp"
#include "client/PacketDispatcher.hpp"
#include "client/CacheDatabaseProxy.hpp"

namespace Client {

    CacheDatabaseProxy::CacheDatabaseProxy(Network& network, Tools::Gui::EventManager& eventManager) :
        _network(network),
        _eventManager(eventManager),
        _worldVersion(0),
        _connectionPool(0)
    {
    }

    CacheDatabaseProxy::~CacheDatabaseProxy()
    {
        Tools::Delete(this->_connectionPool);
    }

    void CacheDatabaseProxy::_CheckCacheFile(std::string const& file)
    {
        if (!boost::filesystem::exists(file))
        {
            std::cout << "CacheDatabaseProxy::_CheckCacheFile: Cache file not found." << std::endl;
            return;
        }
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
            std::cerr << "CacheDatabaseProxy::_CheckCacheFile: Error: " << e.what() << ". Deleting cache file." << std::endl;
            boost::filesystem::remove(file);
            return;
        }
        std::cout << "CacheDatabaseProxy::_CheckCacheFile: Cache file OK." << std::endl;
    }

    void CacheDatabaseProxy::Load(std::string const& host, std::string const& worldIdentifier, Uint32 worldVersion, std::string const& worldName)
    {
        this->_worldVersion = worldVersion;
        this->_worldName = worldName;
        std::string cacheFile = std::string(CACHE_DIR) + "/" + host + "-" + worldIdentifier + ".tccache";
        this->_CheckCacheFile(cacheFile);

        Uint32 cacheVersion;
        std::string error;
        this->_connectionPool = new Tools::Database::ConnectionPool<Tools::Database::Sqlite::Connection>(cacheFile);
        try
        {
            auto conn = this->_connectionPool->GetConnection();
            auto& curs = conn->GetCursor();
            if (conn->HasTable("cache"))
            {
                curs.Execute("SELECT version FROM cache");
                if (curs.HasData())
                    cacheVersion = curs.FetchOne()[0].GetInt();
                else
                    throw std::runtime_error("corrupted cache");
            }
            else
            {
                curs.Execute("CREATE TABLE cache (version INTEGER, format_version INTEGER, world_name TEXT)");
                curs.Execute("INSERT INTO cache (version, format_version) VALUES (0, ?)").Bind(CacheFormatVersion);
                curs.Execute("CREATE TABLE resource (id INTEGER, type TEXT, data BLOB, plugin_id INTEGER, filename TEXT)");
                cacheVersion = 0;
            }
        }
        catch (std::exception& e)
        {
            error = e.what();
        }

        if (error.empty())
        {
            std::cout << "CacheDatabaseProxy::Load: Cache version " << cacheVersion << ", asking for resources..." << std::endl;
            auto p = PacketCreator::GetNeededResourceIdsPacket(cacheVersion);
            this->_network.SendPacket(std::move(p));
        }
        else
        {
            Tools::Delete(this->_connectionPool);
            this->_connectionPool = 0;
            std::cerr << "CacheDatabaseProxy::Load: Error: \"" << error << "\"." << std::endl;
        }
    }

    void CacheDatabaseProxy::AddResource(Common::Resource& res)
    {
        if (!this->_connectionPool)
        {
            std::cerr << "CacheDatabaseProxy::AddResource: Adding resource to an unloaded cache." << std::endl;
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
        if (!this->_connectionPool)
        {
            std::cerr << "CacheDatabaseProxy::GetResource: Get resource on an unloaded cache." << std::endl;
            return 0;
        }
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
                        row[4].GetBlob().size);
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
        if (!this->_connectionPool)
        {
            std::cerr << "CacheDatabaseProxy::ValidateUpdate: Asking for cache validation with cache not loaded." << std::endl;
            return;
        }
        auto conn = this->_connectionPool->GetConnection();
        auto& curs = conn->GetCursor();
        curs.Execute("UPDATE cache SET version = ?, world_name = ?").Bind(this->_worldVersion).Bind(this->_worldName);
        std::cout << "CacheDatabaseProxy::ValidateUpdate: Cache validated for version "
                  << this->_worldVersion << " \"" << this->_worldName << "\"." << std::endl;
        this->_eventManager.Notify<Tools::Gui::Event>("cache/loaded");
    }

}
