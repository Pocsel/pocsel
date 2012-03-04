#include "server/database/ResourceManager.hpp"

#include "common/Packet.hpp"
#include "common/Resource.hpp"

#include "tools/database/ConnectionPool.hpp"

#include "tools/database/sqlite/Connection.hpp"

#include "server/Server.hpp"
#include "server/Settings.hpp"
#include "server/Logger.hpp"

namespace Server { namespace Database {

    ResourceManager::ResourceManager(Server& server) :
        _server(server)
    {
        this->_connectionPool = new Tools::Database::ConnectionPool<Tools::Database::Sqlite::Connection>(
                server.GetSettings().worldFile.string()
                );

        auto conn = this->_connectionPool->GetConnection();
        auto& curs = conn->GetCursor();

        curs.Execute("SELECT count(*) FROM resource");
        size_t nbResources = curs.FetchOne()[0].GetInt() + 1;
        this->_resources.resize(nbResources);
        curs.Execute("SELECT id, plugin_id, version, type, filename, data FROM resource");
        if (!curs.HasData())
            Tools::error << "No resource found on the server !\n";
        while (curs.HasData())
        {
            auto& row = curs.FetchOne();
            Uint32 id = row[0].GetInt();
            Uint32 plugin_id = row[1].GetInt();
            Uint32 version = row[2].GetInt();
            std::string type = row[3].GetString();
            std::string filename = row[4].GetString();
            Tools::Database::Blob data = row[5].GetBlob();

            this->_resources[id] = new Common::Resource(
                id,
                plugin_id,
                type,
                filename,
                data.data,
                static_cast<Uint32>(data.size)
            );
            Log::load << "Found resource " << id << ": " << filename << ".\n";
            this->_ids[filename] = id;
            this->_idsByVersion[version].push_back(id);
        }
    }

    ResourceManager::~ResourceManager()
    {
        std::for_each(this->_resources.begin(), this->_resources.end(), [](Common::Resource* r) { Tools::Delete(r); });
        Tools::Delete(this->_connectionPool);
    }

    Uint32 ResourceManager::GetId(std::string const& filename) const
    {
        auto it = this->_ids.find(filename);
        if (it != this->_ids.end())
            return it->second;
        return 0;
    }

    std::vector<Uint32> ResourceManager::GetNeededResourceIds(Uint32 fromVersion) const
    {
        std::vector<Uint32> ids;
        for (auto mit = this->_idsByVersion.begin(), mite = this->_idsByVersion.end(); mit != mite; ++mit)
        {
            if (mit->first > fromVersion)
            {
                for (auto it = mit->second.begin(), ite = mit->second.end(); it != ite; ++it)
                    ids.push_back(*it);
            }
        }

        return ids;
    }

}}
