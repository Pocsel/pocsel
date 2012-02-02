
#include <cassert>
#include <iostream>

#include "server/ResourceManager.hpp"

using namespace Server;

ResourceManager::ResourceManager(Tools::Database::IConnectionPool& pool) :
    _pool(pool)
{
    auto conn = this->_pool.GetConnection();
    auto& curs = conn->GetCursor();
    // Il faut virer ça mais je sais pas pouquoi (david)
//    curs.Execute("SELECT id, name FROM resource_type");
//    while (curs.HasData())
//    {
//        auto& row = curs.FetchOne();
//        Uint32 id = row[0].GetInt();
//        std::string name = row[1].GetString();
//        std::cout << "Found resource type " << id << ": " << name << ".\n";
//        this->_types[id] = name;
//    }

    curs.Execute("SELECT count(*) FROM resource");
    size_t nbResources = curs.FetchOne()[0].GetInt() + 1;
    this->_resources.resize(nbResources);
    curs.Execute("SELECT id, plugin_id, version, type, filename, data FROM resource");
    if (!curs.HasData())
        std::cerr << "No resource found on the server !\n";
    while (curs.HasData())
    {
        auto& row = curs.FetchOne();
        Uint32 id = row[0].GetInt(),
               plugin_id = row[1].GetInt();
        std::string type = row[3].GetString(),
                    filename = row[4].GetString();
        Tools::Database::Blob data = row[5].GetBlob();

        this->_resources[id] = new Common::Resource(
            id,
            plugin_id,
            type,
            filename,
            data.data,
            data.size
        );
        std::cout << "Found resource " << id << ": " << filename << ".\n";
    }
}
ResourceManager::~ResourceManager()
{
    auto it = this->_resources.begin(),
         end = this->_resources.end();
    for (; it != end; ++it)
        delete *it;
}

Uint32 ResourceManager::GetId(std::string const& filename)
{
    auto it = this->_ids.find(filename);
    Uint32 id = 0;
    if (it == this->_ids.end())
    {
        auto conn = this->_pool.GetConnection();
        auto& curs = conn->GetCursor();
        curs.Execute("SELECT id FROM resource WHERE filename = ?").Bind(filename);
        if (curs.HasData())
        {
            id = curs.FetchOne()[0].GetInt();
            this->_ids[filename] = id;
        }
    }
    else
        id = it->second;
    return id;
}


std::vector<Uint32> ResourceManager::GetNeededResourceIds(Uint32 fromVersion)
{
    auto conn = this->_pool.GetConnection();
    auto& curs = conn->GetCursor();
    curs.Execute("SELECT id FROM resource WHERE version >= ?").Bind(fromVersion);
    std::vector<Uint32> ids;
    while (curs.HasData())
        ids.push_back(curs.FetchOne()[0].GetInt());
    return ids;
}
