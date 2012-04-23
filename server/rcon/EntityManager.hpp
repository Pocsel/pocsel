#ifndef __SERVER_RCON_ENTITYMANAGER_HPP__
#define __SERVER_RCON_ENTITYMANAGER_HPP__

namespace Server {
    class Server;
}

namespace Server { namespace Rcon {

    class EntityManager :
        private boost::noncopyable
    {
    private:
        struct EntityFile
        {
            Uint32 pluginId;
            std::string file;
            std::string lua;
        };
        struct EntityType
        {
            Uint32 pluginId;
            std::string name;
            bool positional;
        };

    private:
        Server& _server;
        std::list<EntityFile> _files;
        std::list<EntityType> _types;

    public:
        EntityManager(Server& server);
        // files
        void AddFile(Uint32 pluginId, std::string const& file, std::string const& lua);
        bool UpdateFile(std::string const& pluginIdentifier,  std::string const& file, std::string const& lua);
        std::string GetFile(std::string const& pluginIdentifier, std::string const& file) const; // retourne "" si erreur
        std::string RconGetEntityFiles() const;
        // types
        void AddType(Uint32 pluginId, std::string const& name, bool positional);
        std::string RconGetEntityTypes() const;
    };

}}

#endif
