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
        Engine& _engine;
        Uint32 _id;
        EntityType const& _type;
        Tools::Lua::Ref _self;
        Uint32 _weakReferenceId;

    public:
        Entity(Engine& interpreter, Uint32 id, EntityType const& type);
        virtual ~Entity();
        EntityType const& GetType() const { return this->_type; }
        Tools::Lua::Ref const& GetSelf() const { return this->_self; }
        Tools::Lua::Ref GetStorage() const;
        void SetStorage(Tools::Lua::Ref const& storage);
        void Disable();
        void Enable();
        void SaveToStorage();
        void LoadFromStorage();
        Uint32 GetWeakReferenceId() const { return this->_weakReferenceId; }
    };

}}}

#endif
