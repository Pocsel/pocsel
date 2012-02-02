#ifndef __CLIENT_RESOURCEMANAGER_HPP__
#define __CLIENT_RESOURCEMANAGER_HPP__

#include <boost/noncopyable.hpp>
#include <map>

namespace Tools {
    class IRenderer;
    namespace Gui {
        class EventManager;
    }
    namespace Renderers {
        class ITexture2D;
    }
}

namespace Client
{

    class Network;
    class PacketDispatcher;
    class CacheDatabaseProxy;
    class ResourceDownloader;

    class ResourceManager :
        private boost::noncopyable
    {
        private:
            CacheDatabaseProxy& _database;
            Tools::IRenderer& _renderer;
            ResourceDownloader* _downloader;
            std::map<Uint32, Tools::Renderers::ITexture2D*> _textures;

        public:
            ResourceManager(Tools::Gui::EventManager& eventManager,
                    Network& network,
                    Tools::IRenderer& renderer,
                    PacketDispatcher& packetDispatcher,
                    CacheDatabaseProxy& database);
            ~ResourceManager();
            Tools::Renderers::ITexture2D* GetTexture2D(Uint32 id);
        private:
            void _InitErrorTexture();
    };

}

#endif
