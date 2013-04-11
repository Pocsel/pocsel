#ifndef __CLIENT_RESOURCES_LOCALRESOURCEMANAGER_HPP__
#define __CLIENT_RESOURCES_LOCALRESOURCEMANAGER_HPP__

#include "tools/sound/fmod/SoundSystem.hpp"

namespace Tools {
    namespace Gfx {
        namespace Effect {
            class Effect;
            class EffectManager;
        }
        namespace Utils {
            class Font;
        }
        class IRenderer;
        class ITexture2D;
    }
    namespace Models {
        class MqmModel;
    }
    namespace Sound {
        class ISoundSystem;
        class ISound;
        }
    }
namespace Client {
    class Client;
}

namespace Client { namespace Resources {

    class LocalResourceManager
        : private boost::noncopyable
    {
        private:
            Client& _client;
            Tools::Gfx::IRenderer& _renderer;
            Tools::Gfx::Effect::EffectManager& _effectManager;
            Tools::Sound::ISoundSystem const& _soundSystem;
            std::map<std::string, Tools::Gfx::Utils::Font*> _fonts;
            std::map<std::string, Tools::Gfx::Effect::Effect*> _shaders;
            std::map<std::string, Tools::Gfx::ITexture2D*> _textures;
            std::map<std::string, Tools::Models::MqmModel*> _models;
            std::map<std::string, Tools::Sound::ISound*> _sounds;

        public:
            LocalResourceManager(Client& client);
            ~LocalResourceManager();

            Tools::Gfx::Utils::Font& GetFont(std::string const& path, Uint32 size);
            Tools::Gfx::ITexture2D& GetTexture2D(std::string const& path);
            Tools::Gfx::Effect::Effect& GetShader(std::string const& path);
            Tools::Models::MqmModel const& GetMqmModel(std::string const& path);
            Tools::Sound::ISound const& GetSound(std::string const& path);

            Tools::Gfx::IRenderer& GetRenderer() { return this->_renderer; }
            Tools::Sound::ISoundSystem const& GetSoundSystem() { return this->_soundSystem; }

        private:
            void _InitErrorModel();
            void _InitErrorTexture();
            Tools::Gfx::ITexture2D& _GetTexture2D(std::string const& path0, std::string const& path);
    };

}}

#endif
