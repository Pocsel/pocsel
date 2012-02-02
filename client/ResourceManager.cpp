#include "ResourceManager.hpp"
#include "CacheDatabaseProxy.hpp"
#include "ResourceDownloader.hpp"
#include "tools/IRenderer.hpp"
#include "common/Resource.hpp"

namespace Client
{

    ResourceManager::ResourceManager(Tools::Gui::EventManager& eventManager,
            Network& network,
            Tools::IRenderer& renderer,
            PacketDispatcher& packetDispatcher,
            CacheDatabaseProxy& database) :
        _database(database), _renderer(renderer)
    {
        this->_downloader = new ResourceDownloader(eventManager, network, packetDispatcher, database);
    }

    ResourceManager::~ResourceManager()
    {
        Tools::Delete(this->_downloader);
    }

    Tools::Renderers::ITexture2D* ResourceManager::GetTexture2D(Uint32 id)
    {
        auto it = this->_textures.find(id);
        if (it == this->_textures.end())
        {
            Common::Resource* res = this->_database.GetResource(id);
            if (res)
            {
                Tools::Renderers::ITexture2D* t = this->_renderer.CreateTexture2D(Tools::Renderers::PixelFormat::Png, res->size, res->data).release();
                t->SetFilters(Tools::Renderers::TextureFilter::Linear, Tools::Renderers::TextureFilter::Nearest);
                this->_textures[id] = t;
                return t;
            }
            else
            {
                if (this->_textures.find(0) == this->_textures.end())
                    this->_InitErrorTexture();
                this->_textures[id] = this->_textures[0];
                return this->_textures[id];
            }
        }
        else
            return it->second;
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
        this->_textures[0]->SetFilters(Tools::Renderers::TextureFilter::Nearest, Tools::Renderers::TextureFilter::Nearest);
    }

}
