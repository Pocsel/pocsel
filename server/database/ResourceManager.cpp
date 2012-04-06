#include "server/precompiled.hpp"

#include "server/database/ResourceManager.hpp"

#include "common/Packet.hpp"
#include "common/Resource.hpp"

#include "tools/database/sqlite/Connection.hpp"

#include "server/Server.hpp"
#include "server/Settings.hpp"
#include "server/Logger.hpp"

namespace Server { namespace Database {

    ResourceManager::ResourceManager(Server& server) :
        _server(server)
    {
        this->_connection = new Tools::Database::Sqlite::Connection(server.GetSettings().worldFile.string());

        Uint32 nbResources = this->_connection->CreateQuery("SELECT count(*) FROM resource")->Fetch()->GetUint32(0);
        if (nbResources == 0)
            Tools::error << "No resource found on the server !\n";
        this->_resources.resize(nbResources + 1);
        auto query = this->_connection->CreateQuery("SELECT id, plugin_id, version, type, filename, data FROM resource");
        while (auto row = query->Fetch())
        {
            Uint32 id = row->GetUint32(0);
            Uint32 plugin_id = row->GetUint32(1);
            Uint32 version = row->GetUint32(2);
            std::string type = row->GetString(3);
            std::string filename = row->GetString(4);
            auto data = row->GetArray(5);

            this->_resources[id] = new Common::Resource(
                id,
                plugin_id,
                type,
                filename,
                data.data(),
                static_cast<Uint32>(data.size())
            );
            Log::load << "Found resource " << id << ": " << filename << ".\n";
            this->_ids[plugin_id][filename] = id;
            this->_idsByVersion[version].push_back(id);
        }
    }

    ResourceManager::~ResourceManager()
    {
        std::for_each(this->_resources.begin(), this->_resources.end(), [](Common::Resource* r) { Tools::Delete(r); });
        Tools::Delete(this->_connection);
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
