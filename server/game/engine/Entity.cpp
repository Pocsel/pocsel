#include "server/game/engine/Entity.hpp"
#include "server/game/engine/EntityType.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Iterator.hpp"
#include "server/game/engine/Engine.hpp"

namespace Server { namespace Game { namespace Engine {

    Entity::Entity(Engine& engine, Uint32 id, EntityType const& type) :
        _engine(engine), _id(id), _type(type), _self(engine.GetInterpreter().MakeTable())
    {
        this->_self.Set("storage", this->_engine.GetInterpreter().MakeTable());
        this->_weakReferenceId = this->_engine.GetEntityManager().GetWeakEntityRefManager().NewResource(EntityManager::WeakEntityRef(id)).first;
        this->Enable();
    }

    Entity::~Entity()
    {
        this->_engine.GetEntityManager().GetWeakEntityRefManager().InvalidateResource(this->_weakReferenceId);
    }

    Tools::Lua::Ref Entity::GetStorage() const
    {
        assert(this->_self.IsTable());
        return this->_self["storage"];
    }

    void Entity::SetStorage(Tools::Lua::Ref const& storage)
    {
        assert(this->_self.IsTable());
        this->_self.Set("storage", storage);
    }

    void Entity::Disable()
    {
        assert(this->_self.IsTable());
        Tools::Lua::Ref storage = this->_self["storage"];
        this->_self = this->_engine.GetInterpreter().MakeTable(); // perte de toutes les references lua
        this->_self.Set("storage", storage); // garde uniquement le storage
        this->_engine.GetEntityManager().GetWeakEntityRefManager().GetResource(this->_weakReferenceId).disabled = true;
    }

    void Entity::Enable()
    {
        assert(this->_self.IsTable());
        this->_self.Set("id", this->_id);
        Tools::Lua::Ref metatable(this->_engine.GetInterpreter().MakeTable());
        this->_self.SetMetaTable(metatable);
        this->_self.Set("prototype", this->_type.GetPrototype());
        metatable.Set("__index", this->_type.GetPrototype());
        this->_engine.GetEntityManager().GetWeakEntityRefManager().GetResource(this->_weakReferenceId).disabled = false;
    }

    void Entity::SaveToStorage()
    {
        assert(this->_self.IsTable());
        if (!this->_self["storage"].IsTable())
            this->_self.Set("storage", this->_self.GetState().MakeTable());
        Tools::Lua::Ref storage = this->_self["storage"];
        auto it = this->_self.Begin();
        auto itEnd = this->_self.End();
        for (; it != itEnd; ++it)
            if (!it.GetKey().Equals("id") && !it.GetKey().Equals("prototype") && !it.GetKey().Equals("storage"))
                storage.Set(it.GetKey(), it.GetValue());
    }

    void Entity::LoadFromStorage()
    {
        assert(this->_self.IsTable());
        if (!this->_self["storage"].IsTable())
            return;
        auto it = this->_self["storage"].Begin();
        auto itEnd = this->_self["storage"].End();
        for (; it != itEnd; ++it)
            if (!it.GetKey().Equals("id") && !it.GetKey().Equals("prototype") && !it.GetKey().Equals("storage"))
                this->_self.Set(it.GetKey(), it.GetValue());
    }

}}}
