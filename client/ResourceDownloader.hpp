#ifndef __CLIENT_RESOURCEDOWNLOADER_HPP__
#define __CLIENT_RESOURCEDOWNLOADER_HPP__

#include <boost/noncopyable.hpp>
#include <map>
#include <list>

namespace Common {
    class Packet;
    class Resource;
}

namespace Tools { namespace Gui {
    class EventManager;
}}

namespace Client {

    class Network;
    class PacketDispatcher;
    class CacheDatabaseProxy;

    class ResourceDownloader :
        private boost::noncopyable
    {
        private:
            Tools::Gui::EventManager& _eventManager;
            CacheDatabaseProxy& _database;
            Network& _network;
            PacketDispatcher& _packetDispatcher;
            std::list<Uint32> _neededResourceIds;
            Uint32 _nbNeededResources;
            Uint32 _nbDownloadedResources;
            std::map<Uint32, Common::Resource*> _inTransfer;

        public:
            ResourceDownloader(Tools::Gui::EventManager& eventManager,
                    Network& network,
                    PacketDispatcher& packetDispatcher,
                    CacheDatabaseProxy& database);
        private:
            void _HandleNeededResourceIds(Common::Packet& p);
            void _HandleResourceRange(Common::Packet& p);
            void _RequestNextResource();
            void _CheckDownloadEnd();
    };

}

#endif
