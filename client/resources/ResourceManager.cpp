#include "client/precompiled.hpp"

#include <IL/il.h>

#include "client/Client.hpp"
#include "client/network/Network.hpp"
#include "client/network/PacketCreator.hpp"
#include "client/resources/CacheDatabaseProxy.hpp"
#include "client/resources/ResourceDownloader.hpp"
#include "client/resources/ResourceManager.hpp"
#include "client/resources/ITexture.hpp"
#include "client/resources/AnimatedTexture.hpp"
#include "client/resources/Effect.hpp"
#include "client/resources/Texture.hpp"
#include "client/window/Window.hpp"
#include "client/Settings.hpp"

#include "tools/Color.hpp"
#include "tools/IRenderer.hpp"
#include "tools/renderers/utils/TextureAtlas.hpp"

#include "common/Resource.hpp"

namespace {
    inline void InitializeFrames(Tools::IRenderer& renderer, void const* data, std::size_t dataSize, std::vector<Tools::Renderers::ITexture2D*>& frames)
    {
        ilDisable(IL_BLIT_BLEND);

        ILuint id = ilGenImage();
        ilBindImage(id);

        if (!ilLoadL(IL_PNG, data, (ILuint)dataSize) && !ilLoadL(IL_TYPE_UNKNOWN, data, (ILuint)dataSize))
        {
            ilBindImage(0);
            ilDeleteImage(id);
            throw std::runtime_error("AnimatedTexture: Can't load data");
        }
        int bytesPerPixel = ilGetInteger(IL_IMAGE_BPP);
        if (bytesPerPixel != 3 && bytesPerPixel != 4)
        {
            ilBindImage(0);
            ilDeleteImage(id);
            throw std::runtime_error("AnimatedTexture: A texture must be 24 or 32 bits per pixels.");
        }

        auto size = glm::uvec2((ILuint)ilGetInteger(IL_IMAGE_WIDTH), (ILuint)ilGetInteger(IL_IMAGE_HEIGHT));
        if (size.h % size.w != 0)
        {
            ilBindImage(0);
            ilDeleteImage(id);
            throw std::runtime_error("AnimatedTexture: height must be a multiple of width.");
        }

        auto frameSize = glm::uvec2(size.w);
        auto pixmap = new Tools::Color4<Uint8>[frameSize.w * frameSize.h];
        frames.resize(size.h / size.w);
        for (unsigned int y = 0, i = 0; y < size.h; y += size.w, ++i)
        {
            ilCopyPixels(0, y, 0, frameSize.w, frameSize.h, 1, IL_RGBA, IL_UNSIGNED_BYTE, pixmap);
            frames[i] = renderer.CreateTexture2D(Tools::Renderers::PixelFormat::Rgba8, frameSize.w * frameSize.h * 4, pixmap, frameSize, 0).release();
        }
        Tools::Delete(pixmap);
        ilBindImage(0);
        ilDeleteImage(id);
    }
}

namespace Client { namespace Resources {

    ResourceManager::ResourceManager(Game::Game& game, std::string const& host, std::string const& worldIdentifier, std::string const& worldName, Uint32 worldVersion, std::string const& worldBuildHash) :
        _game(game),
        _database(game.GetClient().GetSettings().cacheDir, host, worldIdentifier, worldName, worldVersion, worldBuildHash),
        _downloader(_database, game.GetClient().GetNetwork()),
        _renderer(game.GetClient().GetWindow().GetRenderer())
    {
        Tools::log << "Cache version " << this->_database.GetCacheVersion() << ", asking for resources...\n";
        game.GetClient().GetNetwork().SendPacket(Network::PacketCreator::GetNeededResourceIds(this->_database.GetCacheVersion()));
    }

    ResourceManager::~ResourceManager()
    {
        Tools::Renderers::ITexture2D* errTex = 0;
        if (this->_textures.find(0) != this->_textures.end())
            errTex = this->_textures[0];
        Tools::Delete(errTex);
        for (auto it = this->_textures.begin(), ite = this->_textures.end(); it != ite; ++it)
            if (it->second != errTex)
                Tools::Delete(it->second);
        for (auto it = this->_animatedTextures.begin(), ite = this->_animatedTextures.end(); it != ite; ++it)
            for (unsigned int i = 0; i < it->second.size(); ++i)
                if (it->second[i] != errTex)
                    Tools::Delete(it->second[i]);
        for (auto it = this->_shaders.begin(), ite = this->_shaders.end(); it != ite; ++it)
            Tools::Delete(it->second);
    }

    Tools::Renderers::ITexture2D& ResourceManager::GetTexture2D(Uint32 id)
    {
        auto it = this->_textures.find(id);
        if (it == this->_textures.end())
        {
            auto res = this->_database.GetResource(id);
            if (res != 0)
            {
                this->_resourceToPluginId[id] = res->pluginId;
                Tools::Renderers::ITexture2D* t = this->_renderer.CreateTexture2D(Tools::Renderers::PixelFormat::Png, res->size, res->data).release();
                this->_textures[id] = t;
                return *t;
            }
            else
            {
                if (this->_textures.find(0) == this->_textures.end())
                    this->_InitErrorTexture();
                this->_textures[id] = this->_textures[0];
                return *(this->_textures[id]);
            }
        }
        else
            return *it->second;
    }

    std::vector<Tools::Renderers::ITexture2D*> const& ResourceManager::GetAnimatedTexture(Uint32 id)
    {
        auto it = this->_animatedTextures.find(id);
        if (it == this->_animatedTextures.end())
        {
            auto res = this->_database.GetResource(id);
            if (res != 0)
            {
                auto& frames = this->_animatedTextures[id];
                InitializeFrames(this->_renderer, res->data, res->size, frames);
                return frames;
            }
            else
            {
                if (this->_textures.find(0) == this->_textures.end())
                    this->_InitErrorTexture();
                this->_animatedTextures[id].push_back(this->_textures[0]);
                return this->_animatedTextures[id];
            }
        }
        return it->second;
    }

    Tools::Renderers::IShaderProgram& ResourceManager::GetShader(Uint32 id)
    {
        auto it = this->_shaders.find(id);
        if (it == this->_shaders.end())
        {
            auto res = this->_database.GetResource(id);
            if (res != 0)
            {
                this->_resourceToPluginId[id] = res->pluginId;
                Tools::Renderers::IShaderProgram* s = this->_renderer.CreateProgram(std::string((char const*)res->data, res->size)).release();
                this->_shaders[id] = s;
                return *s;
            }
            else
                throw std::runtime_error("Bad id (remove your cache)");
        }
        else
            return *it->second;
    }

    std::string ResourceManager::GetScript(Uint32 id)
    {
        auto it = this->_scripts.find(id);
        if (it == this->_scripts.end())
        {
            auto res = this->_database.GetResource(id);
            if (res != 0)
            {
                this->_resourceToPluginId[id] = res->pluginId;
                std::string str((char const*)res->data, res->size);
                this->_scripts[id] = str;
                return str;
            }
            else
                throw std::runtime_error("Bad id (remove your cache)");
        }
        else
            return it->second;
    }

    std::unique_ptr<Common::Resource> ResourceManager::GetResource(Uint32 id)
    {
        return this->_database.GetResource(id);
    }

    Tools::Renderers::ITexture2D& ResourceManager::GetTexture2D(Uint32 pluginId, std::string const& filename)
    {
        return this->GetTexture2D(this->_database.GetResourceId(pluginId, filename));
    }

    std::vector<Tools::Renderers::ITexture2D*> const& ResourceManager::GetAnimatedTexture(Uint32 pluginId, std::string const& filename)
    {
        return this->GetAnimatedTexture(this->_database.GetResourceId(pluginId, filename));
    }

    Tools::Renderers::IShaderProgram& ResourceManager::GetShader(Uint32 pluginId, std::string const& filename)
    {
        return this->GetShader(this->_database.GetResourceId(pluginId, filename));
    }

    std::string ResourceManager::GetScript(Uint32 pluginId, std::string const& filename)
    {
        return this->GetScript(this->_database.GetResourceId(pluginId, filename));
    }

    std::unique_ptr<Common::Resource> ResourceManager::GetResource(Uint32 pluginId, std::string const& filename)
    {
        return this->_database.GetResource(this->_database.GetResourceId(pluginId, filename));
    }

    Effect& ResourceManager::GetEffect(Uint32 pluginId, std::string const& name)
    {
        auto it = this->_effects.find(pluginId);
        if (it == this->_effects.end())
            throw std::runtime_error("Bad pluginId (remove your cache)");
        auto itEffect = it->second.find(name);
        if (itEffect == it->second.end())
            throw std::runtime_error("Bad pluginId (remove your cache)");
        return *itEffect->second;
    }

    std::unique_ptr<ITexture> ResourceManager::CreateTexture(Uint32 id)
    {
        auto res = this->_database.GetResource(id);
        if (res == 0 || res->type == "image")
            return std::unique_ptr<ITexture>(new Texture(this->GetTexture2D(id)));
        return std::unique_ptr<ITexture>(new AnimatedTexture(this->_game, res->pluginId, this->GetScript(id)));
    }

    std::unique_ptr<Tools::Renderers::Utils::TextureAtlas> ResourceManager::CreateTextureAtlas(std::list<Uint32> const& textureIds)
    {
        std::list<std::unique_ptr<Common::Resource>> resources;
        std::map<Uint32, std::pair<std::size_t, void const*>> textures;
        for (auto it = textureIds.begin(), ite = textureIds.end(); it != ite; ++it)
        {
            auto res = this->_database.GetResource(*it);
            if (res == 0)
                textures[*it] = std::pair<std::size_t, void const*>(0, (void const*)0);
            else
                textures[*it] = std::pair<std::size_t, void const*>(res->size, res->data);
            resources.push_back(std::move(res));
        }
        return std::unique_ptr<Tools::Renderers::Utils::TextureAtlas>(new Tools::Renderers::Utils::TextureAtlas(this->_renderer, textures));
    }

    void ResourceManager::_InitErrorTexture()
    {
        unsigned char toto[] = {
            0, 0, 0, 255,
            255, 0, 255, 255,
            255, 0, 255, 255,
            0, 0, 0, 255
        };
        this->_textures[0] = this->_renderer.CreateTexture2D(Tools::Renderers::PixelFormat::Rgba8, 100312, toto, glm::uvec2(2, 2)).release();
    }

    void ResourceManager::LoadAllResources()
    {
        this->_LoadEffects();
    }

    void ResourceManager::_LoadEffects()
    {
        auto const& resources = this->_database.GetAllResources("lua");

        auto& interpreter = this->_game.GetInterpreter();
        auto clientNs = interpreter.Globals().Set("Client", interpreter.MakeTable());
        auto effectNs = clientNs.Set("Effect", interpreter.MakeTable());

        for (auto it = resources.begin(), ite = resources.end(); it != ite; ++it)
        {
            auto createEffect =
                [this, &it](Tools::Lua::CallHelper& helper)
                {
                    auto pluginId = (*it)->pluginId;
                    auto effect = new Effect(this->_game, helper.PopArg(), pluginId);
                    this->_effects[pluginId][effect->GetName()] = effect;
                    Tools::log << "Register effect \"" << effect->GetName() << "\" (plugin: " << pluginId << ")." << std::endl;
                };
            effectNs.Set("Create", interpreter.MakeFunction(createEffect));
            interpreter.DoString(std::string((char const*)(*it)->data, (*it)->size));
        }
    }

}}
