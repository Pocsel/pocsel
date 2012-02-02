#ifndef __CLIENT_CACHEDATABASEPROXY_HPP__
#define __CLIENT_CACHEDATABASEPROXY_HPP__

#include <boost/noncopyable.hpp>
#include <string>

#include "tools/database/IConnectionPool.hpp"

namespace Common {
    class Packet; class Resource;
}

namespace Tools { namespace Gui {
    class EventManager;
}}

namespace Client
{

    class Network;

    class CacheDatabaseProxy :
        private boost::noncopyable
    {
        private:
            enum
            {
                CacheFormatVersion = 1
            };

        private:
            Network& _network;
            Tools::Gui::EventManager& _eventManager;
            Uint32 _worldVersion;
            std::string _worldName;
            Tools::Database::IConnectionPool* _connectionPool;

        public:
            CacheDatabaseProxy(Network& network, Tools::Gui::EventManager& eventManager);
            ~CacheDatabaseProxy();
            void Load(std::string const& host, std::string const& worldIdentifier, Uint32 worldVersion, std::string const& worldName);
            void ValidateUpdate();
            void AddResource(Common::Resource& res);
            Common::Resource* GetResource(Uint32 id);
        private:
            void _CheckCacheFile(std::string const& path);
    };

}

#endif
