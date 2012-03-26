#ifndef __SERVER_GAME_ENGINE_ENTITY_HPP__
#define __SERVER_GAME_ENGINE_ENTITY_HPP__

#include "tools/lua/Ref.hpp"

namespace Server { namespace Game { namespace Engine {

    class EntityType;
    class Engine;

    class Entity :
        private boost::noncopyable
    {
    protected:
        EntityType& _type;
        Tools::Lua::Ref _self;
        std::map<Uint64 /* time */, Uint32 /* callback */> _pendingCalls;

    public:
        Entity(Engine& engine, Uint32 id, EntityType& type);
        EntityType const& GetType() const { return this->_type; }
        Tools::Lua::Ref const& GetSelf() const { return this->_self; }
    };

}}}

#endif
