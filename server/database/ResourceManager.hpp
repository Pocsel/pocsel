#ifndef __SERVER_DATABASE_RESOURCEMANAGER_HPP__
#define __SERVER_DATABASE_RESOURCEMANAGER_HPP__

namespace Common {

    class Resource;

}

namespace Tools { namespace Database {

    class IConnection;

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
        Tools::Database::IConnection* _connection;
        std::unordered_map<Uint32, std::unordered_map<std::string, Uint32>> _ids;
        std::vector<Common::Resource*> _resources;
        std::map<Uint32, std::vector<Uint32>> _idsByVersion;

    public:
        ResourceManager(Server& server);
        ~ResourceManager();

        Uint32 GetId(Uint32 pluginId, std::string const& filename) const
        {
            auto itPlugin = this->_ids.find(pluginId);
            if (itPlugin == this->_ids.end())
                throw std::range_error("Plugin Id not found");
            auto it = itPlugin->second.find(filename);
            if (it == itPlugin->second.end())
                throw std::range_error("File not found");
            return it->second;
        }
        std::vector<Common::Resource*> const& GetResources() const { return this->_resources; }

        bool HasResource(Uint32 id) const { return id && this->_resources.size() >= id; }
        Common::Resource const& GetResource(Uint32 id) const { return *this->_resources[id]; }

        std::vector<Uint32> GetNeededResourceIds(Uint32 fromVersion) const;

        // Réservé à la construction des merdes de base, et au thread clientManagement
        Tools::Database::IConnection& GetConnection() { return *this->_connection; }
    };

}}

#endif
