#ifndef __CLIENT_RESOURCES_RESOURCEDOWNLOADER_HPP__
#define __CLIENT_RESOURCES_RESOURCEDOWNLOADER_HPP__

namespace Common {
    class Packet;
    class Resource;
}

namespace Client { namespace Network {
    class Network;
}}

namespace Client { namespace Resources {

    class CacheDatabaseProxy;

    class ResourceDownloader
        : private boost::noncopyable
    {
        private:
            CacheDatabaseProxy& _database;
            Network::Network& _network;
            std::list<Uint32> _neededResourceIds;
            Uint32 _nbNeededResources;
            Uint32 _nbDownloadedResources;
            std::map<Uint32, Common::Resource*> _inTransfer;

        public:
            ResourceDownloader(CacheDatabaseProxy& database, Network::Network& network);

            void AskResources(Uint32 nbResources, std::list<Uint32>& neededResources);
            void HandleResourceRange(Common::Packet& p);
            float GetLoadingProgression() const { return this->_nbDownloadedResources / (float)this->_nbNeededResources; }

        private:
            void _RequestNextResource();
            void _CheckDownloadEnd();
    };

}}

#endif
