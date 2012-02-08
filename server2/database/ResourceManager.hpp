#ifndef __SERVER_DATABASE_RESOURCEMANAGER_HPP__
#define __SERVER_DATABASE_RESOURCEMANAGER_HPP__

namespace Common {

    class Resource;

}

namespace Tools { namespace Database {

    class IConnectionPool;

}}

namespace Server {

    class Server;

}

namespace Server { namespace Database {

    class ResourceManager :
        private boost::noncopyable
    {
    private:
        Server& _server;
        Tools::Database::IConnectionPool* _connectionPool;
        std::unordered_map<std::string, Uint32> _ids;
        std::vector<Common::Resource*> _resources;
        std::map<Uint32, std::vector<Uint32>> _idsByVersion;

    public:
        ResourceManager(Server& server);
        ~ResourceManager();

        Uint32 GetId(std::string const& filename) const;
        std::vector<Common::Resource*> const& GetResources() const { return this->_resources; }

        bool HasResource(Uint32 id) const { return id && this->_resources.size() >= id; }
        Common::Resource const& GetResource(Uint32 id) const { return *this->_resources[id]; }

        std::vector<Uint32> GetNeededResourceIds(Uint32 fromVersion) const;
    };

}}

#endif
