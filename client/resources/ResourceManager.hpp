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
    namespace Lua {
        class CallHelper;
    }
}

namespace Client {
    class Client;
    namespace Game {
        class Game;
    }
    namespace Resources {
        class ITexture;
        class Effect;
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
        std::map<Uint32 /* resourceId */, Uint32 /* pluginId */> _resourceToPluginId;
        std::map<Uint32 /* pluginId */, std::map<std::string /* name */, Uint32 /* resourceId */>> _resourceIds;
        std::map<Uint32 /* pluginId */, Tools::Renderers::ITexture2D*> _textures;
        std::map<Uint32 /* pluginId */, std::vector<Tools::Renderers::ITexture2D*>> _animatedTextures;
        std::map<Uint32 /* pluginId */, Tools::Renderers::IShaderProgram*> _shaders;
        std::map<Uint32 /* pluginId */, std::map<std::string, Effect*>> _effects;
        std::map<Uint32 /* pluginId */, std::string> _scripts;

    public:
        ResourceManager(Game::Game& game,
                        std::string const& host,
                        std::string const& worldIdentifier,
                        std::string const& worldName,
                        Uint32 worldVersion,
                        std::string const& worldBuildHash);
        ~ResourceManager();

        Tools::Renderers::ITexture2D& GetTexture2D(Uint32 id);
        std::vector<Tools::Renderers::ITexture2D*> const& GetAnimatedTexture(Uint32 id);
        Tools::Renderers::IShaderProgram& GetShader(Uint32 id);
        std::string GetScript(Uint32 id);
        std::unique_ptr<Common::Resource> GetResource(Uint32 id);

        Tools::Renderers::ITexture2D& GetTexture2D(Uint32 pluginId, std::string const& name);
        std::vector<Tools::Renderers::ITexture2D*> const& GetAnimatedTexture(Uint32 pluginId, std::string const& name);
        Tools::Renderers::IShaderProgram& GetShader(Uint32 pluginId, std::string const& name);
        std::string GetScript(Uint32 pluginId, std::string const& name);
        std::unique_ptr<Common::Resource> GetResource(Uint32 pluginId, std::string const& name);
        Effect& GetEffect(Uint32 pluginId, std::string const& name);

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
        Uint32 GetResourceId(Uint32 pluginId, std::string const& name)
        {
            auto it = this->_resourceIds.find(pluginId);
            if (it == this->_resourceIds.end())
                return 0;
            auto it2 = it->second.find(name);
            return it2 != it->second.end() ? it2->second : 0;
        }

        void BuildResourceIndex();
        void RegisterLuaFunctions();

    private:
        void _InitErrorTexture();
        void _ApiRegisterEffect(Tools::Lua::CallHelper& helper);
    };

}}

#endif
