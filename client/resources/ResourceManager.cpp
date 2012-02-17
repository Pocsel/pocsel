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
    }

    Tools::Renderers::ITexture2D& ResourceManager::GetTexture2D(Uint32 id)
    {
        auto it = this->_textures.find(id);
        if (it == this->_textures.end())
        {
            Common::Resource* res = this->_database.GetResource(id);
            if (res)
            {
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
