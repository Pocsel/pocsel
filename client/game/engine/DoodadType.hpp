#ifndef __CLIENT_GAME_ENGINE_DOODADTYPE_HPP__
#define __CLIENT_GAME_ENGINE_DOODADTYPE_HPP__

#include "tools/lua/Ref.hpp"

namespace Client { namespace Game { namespace Engine {

    class DoodadType :
        private boost::noncopyable
    {
    private:
        Uint32 _pluginId;
        std::string _name;
        Tools::Lua::Ref _prototype;

    public:
        DoodadType(Uint32 pluginId, std::string const& name, Tools::Lua::Ref const& prototype);
        Tools::Lua::Ref const& GetPrototype() const { return this->_prototype; }
        Uint32 GetPluginId() const { return this->_pluginId; }
        std::string const& GetName() const { return this->_name; }
    };

}}}

#endif
