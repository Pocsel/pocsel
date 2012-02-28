#ifndef __CLIENT_RESOURCES_RESOURCEMANAGER_HPP__
#define __CLIENT_RESOURCES_RESOURCEMANAGER_HPP__

#include "client/resources/CacheDatabaseProxy.hpp"
#include "client/resources/ResourceDownloader.hpp"

namespace Common {
    class Resource;
}
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
    namespace Game {
        class Game;
    }
    namespace Resources {
        class ITexture;
    }
}

namespace Client { namespace Resources {

    class ResourceManager
        : private boost::noncopyable
    {
    private:
        Game::Game& _game;
        CacheDatabaseProxy _database;
        ResourceDownloader _downloader;
        Tools::IRenderer& _renderer;
        std::map<Uint32, Uint32> _resourceToPluginId;
        std::map<Uint32, Tools::Renderers::ITexture2D*> _textures;
        std::map<Uint32, Tools::Renderers::IShaderProgram*> _shaders;
        std::map<Uint32, std::string> _scripts;

    public:
        ResourceManager(Game::Game& game,
                        std::string const& host,
                        std::string const& worldIdentifier,
                        std::string const& worldName,
                        Uint32 worldVersion);
        ~ResourceManager();

        Tools::Renderers::ITexture2D& GetTexture2D(Uint32 id);
        Tools::Renderers::IShaderProgram& GetShader(Uint32 id);
        std::string GetScript(Uint32 id);
        Tools::Renderers::ITexture2D& GetTexture2D(Uint32 pluginId, std::string const& filename);
        Tools::Renderers::IShaderProgram& GetShader(Uint32 pluginId, std::string const& filename);
        std::string GetScript(Uint32 pluginId, std::string const& filename);
        std::unique_ptr<Common::Resource> GetResource(Uint32 pluginId, std::string const& filename);

        std::unique_ptr<ITexture> CreateTexture(Uint32 id);
        std::unique_ptr<Tools::Renderers::Utils::TextureAtlas> CreateTextureAtlas(std::list<Uint32> const& textureIds);

        CacheDatabaseProxy& GetDatabase() { return this->_database; }
        ResourceDownloader& GetDownloader() { return this->_downloader; }
        float GetLoadingProgression() const { return this->_downloader.GetLoadingProgression(); }
        Uint32 GetPluginId(Uint32 resourceId) const
        {
            auto it = this->_resourceToPluginId.find(resourceId);
            return it != this->_resourceToPluginId.end() ? it->second : 0;
        }

    private:
        void _InitErrorTexture();
    };

}}

#endif
