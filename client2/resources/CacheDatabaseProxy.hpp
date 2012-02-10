#ifndef __CLIENT_RESOURCES_CACHEDATABASEPROXY_HPP__
#define __CLIENT_RESOURCES_CACHEDATABASEPROXY_HPP__

#include "tools/database/IConnectionPool.hpp"

namespace Common {
    class Packet;
    class Resource;
}
namespace Client { namespace Resources {

    class ResourceManager;

    class CacheDatabaseProxy
        : private boost::noncopyable
    {
        private:
            enum
            {
                CacheFormatVersion = 1
            };

        private:
            Uint32 _worldVersion;
            std::string _worldName;
            Tools::Database::IConnectionPool* _connectionPool;
            Uint32 _cacheVersion;

        public:
            CacheDatabaseProxy(std::string const& host,
                               std::string const& worldIdentifier,
                               std::string const& worldName,
                               Uint32 worldVersion);
            ~CacheDatabaseProxy();
            void ValidateUpdate();
            void AddResource(Common::Resource& res);
            Common::Resource* GetResource(Uint32 id);
            Uint32 GetCacheVersion() const { return this->_cacheVersion; }
        private:
            void _CheckCacheFile(std::string const& path);
    };

}}

#endif
