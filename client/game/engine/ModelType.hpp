#ifndef __CLIENT_GAME_ENGINE_MODELTYPE_HPP__
#define __CLIENT_GAME_ENGINE_MODELTYPE_HPP__

#include "tools/lua/Ref.hpp"

namespace Client { namespace Game { namespace Engine {

    class ModelType :
        private boost::noncopyable
    {
    private:
        std::string _name;
        Uint32 _pluginId;
        Tools::Lua::Ref _prototype;

    public:
        ModelType(std::string const& name, Uint32 pluginId, Tools::Lua::Ref const& prototype);
        Tools::Lua::Ref const& GetPrototype() const { return this->_prototype; }
        Uint32 GetPluginId() const { return this->_pluginId; }
        std::string const& GetName() const { return this->_name; }
    };

}}}

#endif
