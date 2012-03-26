#ifndef __SERVER_GAME_ENGINE_ENTITYTYPE_HPP__
#define __SERVER_GAME_ENGINE_ENTITYTYPE_HPP__

#include "tools/lua/Ref.hpp"

namespace Server { namespace Game { namespace Engine {

    class EntityType :
        private boost::noncopyable
    {
    private:
        std::string _name;
        Uint32 _pluginId;
        Tools::Lua::Ref _prototype;

    public:
        EntityType(std::string const& name, Uint32 pluginId, Tools::Lua::Ref const& prototype);
        std::string const& GetName() const { return this->_name; }
        Uint32 GetPluginId() const { return this->_pluginId; }
        Tools::Lua::Ref const& GetPrototype() const { return this->_prototype; }
    };

}}}

#endif
