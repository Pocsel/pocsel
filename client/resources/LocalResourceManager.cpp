#include <boost/filesystem/fstream.hpp>

#include "client/precompiled.hpp"

#include "tools/gfx/IRenderer.hpp"
#include "tools/gfx/utils/Font.hpp"
#include "tools/window/Window.hpp"
#include "tools/models/MqmModel.hpp"
#include "tools/models/ErrorModel.hpp"
#include "tools/sound/ISoundSystem.hpp"
#include "tools/sound/ISound.hpp"

#include "client/Client.hpp"
#include "client/Settings.hpp"
#include "client/network/Network.hpp"
#include "client/network/PacketCreator.hpp"
#include "client/resources/LocalResourceManager.hpp"

namespace Client { namespace Resources {

    LocalResourceManager::LocalResourceManager(Client& client) :
        _client(client),
        _renderer(client.GetWindow().GetRenderer()),
        _effectManager(client.GetEffectManager()),
        _soundSystem(client.GetSoundSystem())
    {
        this->_InitErrorTexture();
        this->_InitErrorModel();
    }

    LocalResourceManager::~LocalResourceManager()
    {
        for (auto it = this->_textures.begin(), ite = this->_textures.end(); it != ite; ++it)
            Tools::Delete(it->second);
        for (auto it = this->_shaders.begin(), ite = this->_shaders.end(); it != ite; ++it)
            Tools::Delete(it->second);
        for (auto it = this->_fonts.begin(), ite = this->_fonts.end(); it != ite; ++it)
            Tools::Delete(it->second);
        for (auto it = this->_models.begin(), ite = this->_models.end(); it != ite; ++it)
            Tools::Delete(it->second);
        for (auto it = this->_sounds.begin(), ite = this->_sounds.end(); it != ite; ++it)
            Tools::Delete(it->second);
    }

    Tools::Gfx::Utils::Font& LocalResourceManager::GetFont(std::string const& path, Uint32 size)
    {
        std::string fontId =  path + Tools::ToString(size);
        auto it = this->_fonts.find(fontId);
        if (it == this->_fonts.end())
        {
            try
            {
                auto fontPath = (this->_client.GetSettings().confDir / "fonts" / path).string();
                auto font = new Tools::Gfx::Utils::Font(this->_renderer, fontPath, size);
                this->_fonts[fontId] = font;
                return *font;
            }
            catch (std::exception& ex)
            {
                Tools::error << "Can't load local font \"" << path << "\", details: " << ex.what() << "\n";
                throw;
            }
        }
        else
            return *it->second;
    }

    Tools::Gfx::ITexture2D& LocalResourceManager::GetTexture2D(std::string const& path)
    {
        auto it = this->_textures.find(path);
        if (it == this->_textures.end())
        {
            Tools::Gfx::ITexture2D* texture = 0;
            try
            {
                texture = this->_renderer.CreateTexture2D((this->_client.GetSettings().confDir / "textures" / path).string()).release();
            }
            catch (std::exception& ex)
            {
                Tools::error << "Can't load local texture \"" << path << "\", details: " << ex.what() << "\n";
                texture = this->_textures["__error__"];
            }
            if (texture == this->_textures["__error__"])
                this->_textures[path] = 0;
            else
                this->_textures[path] = texture;
            return *texture;
        }
        else if (it->second != 0)
            return *it->second;
        return *this->_textures["__error__"];
    }

    Tools::Gfx::Effect::Effect& LocalResourceManager::GetShader(std::string const& path)
    {
        auto it = this->_shaders.find(path);
        if (it == this->_shaders.end())
        {
            try
            {
                auto shader = this->_effectManager.CreateEffect((this->_client.GetSettings().confDir / "shaders" / path).string()).release();
                this->_shaders[path] = shader;
                return *shader;
            }
            catch (std::exception& ex)
            {
                Tools::error << "Can't load local shader \"" << path << "\", details: " << ex.what() << "\n";
                throw;
            }
        }
        else
            return *it->second;
    }

    Tools::Models::MqmModel const& LocalResourceManager::GetMqmModel(std::string const& path)
    {
        auto it = this->_models.find(path);
        if (it == this->_models.end())
        {
            Tools::Models::MqmModel* model = 0;
            try
            {
                boost::filesystem::path texturesPath = path;
                boost::filesystem::path modelPath = this->_client.GetSettings().confDir / "models" / path;
                modelPath.replace_extension(".mqm");
                if (!boost::filesystem::exists(modelPath))
                    throw std::runtime_error("GetMqmModel: Failed to find file: " + modelPath.string());
                std::ifstream tmp(modelPath.string(), std::ios::binary | std::ios::in);
                std::vector<char> data((std::istreambuf_iterator<char>(tmp)), std::istreambuf_iterator<char>());

                model = new Tools::Models::MqmModel(
                        data,
                        //std::bind(&LocalResourceManager::_GetTexture2D, this, texturesPath.string(), std::placeholders::_1),
                        this->_renderer
                        );
            }
            catch (std::exception& ex)
            {
                Tools::error << "Can't load MqmModel \"" << path << "\", details: " << ex.what() << "\n";
                model = this->_models["__error__"];
            }
            if (model == this->_models["__error__"])
                this->_models[path] = 0;
            else
                this->_models[path] = model;
            return *model;
        }
        else if (it->second != 0)
            return *it->second;
        return *this->_models["__error__"];
    }

    Tools::Sound::ISound const& LocalResourceManager::GetSound(std::string const& path)
    {
        auto it = this->_sounds.find(path);
        if (it == this->_sounds.end())
        {
            auto soundPath = (this->_client.GetSettings().confDir / "sounds" / path).string();
            Tools::Sound::ISound* sound = this->_soundSystem.CreateSound(soundPath);
            this->_sounds[path] = sound;
            return *sound;
        }
        else
            return *it->second;
    }

    void LocalResourceManager::_InitErrorModel()
    {
        this->_models["__error__"] = Tools::Models::ErrorModel(
                //std::bind(&LocalResourceManager::GetTexture2D, this, std::placeholders::_1),
                this->_renderer
                ).release();
    }

    void LocalResourceManager::_InitErrorTexture()
    {
        static const unsigned char toto[] = {
            0, 0, 0, 255,
            255, 0, 255, 255,
            255, 0, 255, 255,
            0, 0, 0, 255
        };
        this->_textures["__error__"] = this->_renderer.CreateTexture2D(Tools::Gfx::PixelFormat::Rgba8, 4, toto, glm::uvec2(2, 2)).release();
    }

    Tools::Gfx::ITexture2D& LocalResourceManager::_GetTexture2D(std::string const& path0, std::string const& path)
    {
        boost::filesystem::path texturePath(path);
        boost::filesystem::path texturePath0(path0);
        return this->GetTexture2D((texturePath0 / texturePath).string());
    }

}}
