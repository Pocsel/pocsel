#ifndef __SERVER_GAME_ENGINE_ENTITY_HPP__
#define __SERVER_GAME_ENGINE_ENTITY_HPP__

namespace Server { namespace Game { namespace Engine {

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
    };

}}}

#endif
