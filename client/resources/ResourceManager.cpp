#include "client/precompiled.hpp"

#include "client/Client.hpp"
#include "client/network/Network.hpp"
#include "client/network/PacketCreator.hpp"
#include "client/resources/CacheDatabaseProxy.hpp"
#include "client/resources/ResourceDownloader.hpp"
#include "client/resources/ResourceManager.hpp"
#include "client/window/Window.hpp"
#include "client/Settings.hpp"

#include "tools/IRenderer.hpp"
#include "tools/renderers/utils/TextureAtlas.hpp"

#include "common/Resource.hpp"

namespace Client { namespace Resources {

    ResourceManager::ResourceManager(Client& client, std::string const& host, std::string const& worldIdentifier, std::string const& worldName, Uint32 worldVersion) :
        _database(client.GetSettings().cacheDir, host, worldIdentifier, worldName, worldVersion),
        _downloader(_database, client.GetNetwork()),
        _renderer(client.GetWindow().GetRenderer())
    {
        Tools::log << "Cache version " << this->_database.GetCacheVersion() << ", asking for resources...\n";
        client.GetNetwork().SendPacket(Network::PacketCreator::GetNeededResourceIds(this->_database.GetCacheVersion()));
    }

    ResourceManager::~ResourceManager()
    {
        for (auto it = this->_textures.begin(), ite = this->_textures.end(); it != ite; ++it)
            Tools::Delete(it->second);
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

    Tools::Renderers::ITexture2D& ResourceManager::GetTexture2D(Uint32 pluginId, std::string const& filename)
    {
        return this->GetTexture2D(this->_database.GetResourceId(pluginId, filename));
    }

    Tools::Renderers::IShaderProgram& ResourceManager::GetShader(Uint32 pluginId, std::string const& filename)
    {
        return this->GetShader(this->_database.GetResourceId(pluginId, filename));
    }

    std::string ResourceManager::GetScript(Uint32 pluginId, std::string const& filename)
    {
        return this->GetScript(this->_database.GetResourceId(pluginId, filename));
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
        this->_textures[0] = this->_renderer.CreateTexture2D(Tools::Renderers::PixelFormat::Rgba8, 100312, toto, Tools::Vector2u(2, 2)).release();
    }

}}
