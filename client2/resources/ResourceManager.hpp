#ifndef __CLIENT_RESOURCES_RESOURCEMANAGER_HPP__
#define __CLIENT_RESOURCES_RESOURCEMANAGER_HPP__

#include "client2/resources/CacheDatabaseProxy.hpp"
#include "client2/resources/ResourceDownloader.hpp"

namespace Tools {
    class IRenderer;
    namespace Renderers {
        class IShaderProgram;
        class ITexture2D;
    }
}

namespace Client {
    class Client;
}

namespace Client { namespace Resources {

    class ResourceManager
        : private boost::noncopyable
    {
        private:
            CacheDatabaseProxy _database;
            ResourceDownloader _downloader;
            Tools::IRenderer& _renderer;
            std::map<Uint32, Tools::Renderers::ITexture2D*> _textures;

        public:
            ResourceManager(Client& client,
                            std::string const& host,
                            std::string const& worldIdentifier,
                            std::string const& worldName,
                            Uint32 worldVersion);
            ~ResourceManager();

            Tools::Renderers::ITexture2D& GetTexture2D(Uint32 id);
            Tools::Renderers::IShaderProgram& GetShader(Uint32 id);

            CacheDatabaseProxy& GetDatabase() { return this->_database; }
            ResourceDownloader& GetDownloader() { return this->_downloader; }

        private:
            void _InitErrorTexture();
    };

}}

#endif
