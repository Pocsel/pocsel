#ifndef __CLIENT_RESOURCES_RESOURCEMANAGER_HPP__
#define __CLIENT_RESOURCES_RESOURCEMANAGER_HPP__

#include "client/resources/CacheDatabaseProxy.hpp"
#include "client/resources/ResourceDownloader.hpp"
#include "tools/gfx/effect/EffectManager.hpp"
#include "tools/gfx/utils/material/LuaMaterial.hpp"
#include "tools/gfx/utils/texture/ITexture.hpp"

namespace Luasel {
    class CallHelper;
}
namespace Common {
    class Resource;
}
namespace Tools {
    namespace Models {
        class MqmModel;
    }
    namespace Sound {
        class ISound;
        class ISoundSystem;
}
}

namespace Client {
    class Client;
    namespace Game {
        class Game;
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
        Tools::Gfx::IRenderer& _renderer;
        Tools::Gfx::Effect::EffectManager& _effectManager;
        Tools::Sound::ISoundSystem const& _soundSystem;

        // resource name => resource Id
        std::map<std::string, Uint32> _resourceIds;

        // Resources (resource id => specific resource)
        std::map<Uint32, Tools::Gfx::Utils::Texture::ITexture*> _textures;
        std::map<Uint32, Tools::Models::MqmModel*> _models;
        std::map<Uint32, Tools::Gfx::Effect::Effect*> _shaders;
        std::map<Uint32, std::string> _scripts;
        std::map<Uint32, std::shared_ptr<Tools::Sound::ISound>> _sounds;

        // effect c'est pas une resource mais un registrabrle
        std::map<std::string, std::unique_ptr<Tools::Gfx::Utils::Material::LuaMaterial>> _materials;

        // Texture data (resource Id => renderer Texture)
        std::map<Uint32, Tools::Gfx::ITexture2D*> _rawTextures;
        std::map<Uint32, std::vector<Tools::Gfx::ITexture2D*>> _animatedTextureFrames;

    public:
        ResourceManager(Game::Game& game,
                        std::string const& host,
                        std::string const& worldIdentifier,
                        std::string const& worldName,
                        Uint32 worldVersion,
                        std::string const& worldBuildHash);
        ~ResourceManager();

        std::unique_ptr<Tools::Gfx::Utils::Texture::ITexture> GetTexture(Uint32 id);
        Tools::Models::MqmModel const& GetMqmModel(Uint32 id);
        Tools::Gfx::Effect::Effect& GetShader(Uint32 id);
        std::shared_ptr<Tools::Sound::ISound> GetSound(Uint32 id);
        std::string GetScript(Uint32 id);        std::unique_ptr<Common::Resource> GetResource(Uint32 id);

        Tools::Models::MqmModel const& GetMqmModel(std::string const& name);
        Tools::Gfx::Effect::Effect& GetShader(std::string const& name);
        std::shared_ptr<Tools::Sound::ISound> GetSound(std::string const& name);
        std::string GetScript(std::string const& name);        std::unique_ptr<Common::Resource> GetResource(std::string const& name);

        // Les registrables
        std::unique_ptr<Tools::Gfx::Utils::Texture::ITexture> GetTexture(std::string const& name);
        std::unique_ptr<Tools::Gfx::Utils::Material::LuaMaterial> GetMaterial(std::string const& name);

        CacheDatabaseProxy& GetDatabase() { return this->_database; }
        ResourceDownloader& GetDownloader() { return this->_downloader; }
        float GetLoadingProgression() const { return this->_downloader.GetLoadingProgression(); }

        Uint32 GetResourceId(std::string const& name) const
        {
            auto it = this->_resourceIds.find(name);
            return it != this->_resourceIds.end() ? it->second : 0;
        }

        void BuildResourceIndex();
        void RegisterLuaFunctions();

    private:
        void _InitErrorTexture();
        void _InitErrorModel();
        void _InitErrorShader();
        void _InitErrorMaterial();
        void _ApiRegisterMaterial(Luasel::CallHelper& helper);
    };

}}

#endif
