#ifndef __SERVER_GAME_ENGINE_ENTITY_HPP__
#define __SERVER_GAME_ENGINE_ENTITY_HPP__

#include <luasel/Luasel.hpp>

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
        Luasel::Ref _self;
        Uint32 _weakReferenceId;

    public:
        Entity(Engine& interpreter, Uint32 id, EntityType const& type);
        virtual ~Entity();
        EntityType const& GetType() const { return this->_type; }
        Luasel::Ref const& GetSelf() const { return this->_self; }
        Uint32 GetId() const { return this->_id; }
        Luasel::Ref GetStorage() const;
        void SetStorage(Luasel::Ref const& storage);
        void Disable();
        void Enable();
        void SaveToStorage();
        void LoadFromStorage();
        Uint32 GetWeakReferenceId() const { return this->_weakReferenceId; }
    };

}}}

#endif
