#ifndef __SERVER_RESOURCEMANAGER_HPP__
#define __SERVER_RESOURCEMANAGER_HPP__

#include <string>
#include <vector>
#include <unordered_map>

#include <boost/noncopyable.hpp>

#include "tools/database/IConnectionPool.hpp"

#include "common/Resource.hpp"

namespace Server {

    class ResourceManager : private boost::noncopyable
    {
    private:
        Tools::Database::IConnectionPool& _pool;
        std::unordered_map<std::string, Uint32> _ids;
        std::vector<Common::Resource*> _resources;
        std::vector<std::string> _types;

    public:
        ResourceManager(Tools::Database::IConnectionPool& pool);
        ~ResourceManager();
        Uint32 GetId(std::string const& filename);
        Common::Resource const& GetResource(Uint32 id)
        { return *this->_resources[id]; }

        std::vector<Common::Resource*> const& GetResources()
        { return this->_resources; }

        std::string const& GetResourceType(Uint32 id)
        { return this->_types[id]; }

        bool HasResourceType(Uint32 id)
        { return id && this->_types.size() >= id; }

        bool HasResource(Uint32 id)
        { return id && this->_resources.size() >= id; }

        std::vector<Uint32> GetNeededResourceIds(Uint32 fromVersion);
    };

}

#endif
