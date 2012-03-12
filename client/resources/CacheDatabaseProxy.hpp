#ifndef __CLIENT_RESOURCES_CACHEDATABASEPROXY_HPP__
#define __CLIENT_RESOURCES_CACHEDATABASEPROXY_HPP__

namespace Common {
    class Packet;
    class Resource;
}

namespace Tools { namespace Database {
    class IConnection;
}}

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
            Tools::Database::IConnection* _connection;
            Uint32 _cacheVersion;
            std::string _worldBuildHash;

        public:
            CacheDatabaseProxy(boost::filesystem::path const& cacheDir,
                               std::string const& host,
                               std::string const& worldIdentifier,
                               std::string const& worldName,
                               Uint32 worldVersion,
                               std::string const& worldBuildHash);
            ~CacheDatabaseProxy();
            void ValidateUpdate();
            void AddResource(Common::Resource& res);
            std::unique_ptr<Common::Resource> GetResource(Uint32 id);
            Uint32 GetResourceId(Uint32 pluginId, std::string const& filename);
            Uint32 GetCacheVersion() const { return this->_cacheVersion; }
        private:
            void _CheckCacheFile(std::string const& path);
    };

}}

#endif
