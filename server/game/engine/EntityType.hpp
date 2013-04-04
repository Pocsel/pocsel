#ifndef __SERVER_GAME_ENGINE_ENTITYTYPE_HPP__
#define __SERVER_GAME_ENGINE_ENTITYTYPE_HPP__

#include <luasel/Luasel.hpp>

namespace Server { namespace Game { namespace Engine {

    class EntityType :
        private boost::noncopyable
    {
    private:
        std::string _name;
        Uint32 _pluginId;
        Luasel::Ref _prototype;
        bool _isPositional;

    public:
        EntityType(std::string const& name, Uint32 pluginId, Luasel::Ref const& prototype, bool isPositional);
        std::string const& GetName() const { return this->_name; }
        Uint32 GetPluginId() const { return this->_pluginId; }
        Luasel::Ref const& GetPrototype() const { return this->_prototype; }
        void SetPrototype(Luasel::Ref const& prototype) { this->_prototype = prototype; }
        bool IsPositional() const { return this->_isPositional; }
    };

}}}

#endif
