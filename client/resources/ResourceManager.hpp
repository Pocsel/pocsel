#ifndef __CLIENT_RESOURCES_RESOURCEMANAGER_HPP__
#define __CLIENT_RESOURCES_RESOURCEMANAGER_HPP__

#include "client/resources/CacheDatabaseProxy.hpp"
#include "client/resources/ResourceDownloader.hpp"

namespace Tools {
    class IRenderer;
    namespace Renderers {
        class IShaderProgram;
        class ITexture2D;
        namespace Utils {
            class TextureAtlas;
        }
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
            std::unique_ptr<Tools::Renderers::Utils::TextureAtlas> CreateTextureAtlas(std::list<Uint32> const& textureIds);

            CacheDatabaseProxy& GetDatabase() { return this->_database; }
            ResourceDownloader& GetDownloader() { return this->_downloader; }
            float GetLoadingProgression() const { return this->_downloader.GetLoadingProgression(); }

        private:
            void _InitErrorTexture();
    };

}}

#endif
