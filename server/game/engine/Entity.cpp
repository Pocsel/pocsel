#include "server/game/engine/Entity.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/engine/EntityType.hpp"
#include "tools/lua/Interpreter.hpp"

namespace Server { namespace Game { namespace Engine {

    Entity::Entity(Engine& engine, Uint32 id, EntityType* type) :
        _self(engine.GetInterpreter().MakeTable())
    {
        Tools::Lua::Ref metatable = engine.GetInterpreter().MakeTable();
        this->_self.SetMetaTable(metatable);
        this->_self.Set("id", id);
        this->SetType(type);
    }

    Entity::~Entity()
    {
    }

    void Entity::SetType(EntityType* type)
    {
        this->_type = type;
        this->_self.Set("prototype", this->_type->GetPrototype());
        Tools::Lua::Ref metatable = this->_self.GetMetaTable();
        if (metatable.IsTable())
            metatable.Set("__index", this->_type->GetPrototype());
    }

}}}
