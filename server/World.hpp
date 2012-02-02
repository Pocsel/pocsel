#ifndef  __SERVER_WORLD_HPP__
#define __SERVER_WORLD_HPP__

#include <map>
#include <memory>

#include <boost/noncopyable.hpp>

#include "tools/database/IConnectionPool.hpp"

#include "common/CubeType.hpp"

#include "server/ResourceManager.hpp"
#include "server/Map.hpp"


namespace Server {

    class CubeTypeLoader;
    class ConnectionPoll;

    class World : private boost::noncopyable
    {
    private:
        std::unordered_map<std::string, Map*> _maps;
        Map* _defaultMap;
        ResourceManager* _resourceManager;
        CubeTypeLoader* _cubeTypeLoader;
        std::vector<Common::CubeType> _cubeTypes;
        std::string _identifier;
        std::string _fullname;
        Uint32 _version;

    public:
        World(Tools::Database::IConnectionPool& conn);
        ~World();
        //size_t Poll();

        Map& GetMap(std::string const& name)
        {
            auto it = this->_maps.find(name);
            if (it == this->_maps.end())
                throw std::runtime_error("Cannot find map '" + name + "'");
            return *(it->second);
        }

        Map& GetDefaultMap()
        { return *this->_defaultMap; }

        ResourceManager& GetResourceManager()
        { return *this->_resourceManager; }

        std::vector<Common::CubeType>& GetCubeTypes()
        { return this->_cubeTypes; }

        Common::CubeType& GetCubeType(Uint32 id)
        { assert(id != 0); return this->_cubeTypes[id - 1]; }

        bool HasCubeType(Uint32 id)
        { return id && this->_cubeTypes.size() >= id; }

        std::string const& GetIdentifier() const { return this->_identifier; }
        std::string const& GetFullname() const { return this->_fullname; }
        Uint32 GetVersion() const { return this->_version; }

    private:
        Common::CubeType* _GetCubeTypeByName(std::string const& name);
        void _LoadMap(std::string const& name, std::string const& code);
    };

}

#endif
