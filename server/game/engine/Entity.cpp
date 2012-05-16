#include "server/game/engine/Entity.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/engine/EntityType.hpp"
#include "tools/lua/Interpreter.hpp"

namespace Server { namespace Game { namespace Engine {

    Entity::Entity(Engine& engine, Uint32 id, EntityType* type) :
        _type(type), _self(engine.GetInterpreter().MakeTable())
    {
        Tools::Lua::Ref metatable = engine.GetInterpreter().MakeTable();
        this->_self.SetMetaTable(metatable);
        this->_self.Set("id", id);
        this->_self.Set("prototype", this->_type->GetPrototype());
        this->_self.Set("storage", engine.GetInterpreter().MakeTable());
        metatable.Set("__index", this->_type->GetPrototype());
    }

    Entity::~Entity()
    {
    }

    Tools::Lua::Ref Entity::GetStorage() const
    {
        if (this->_self.IsTable())
            return this->_self["storage"];
        return Tools::Lua::Ref(this->_self.GetState()); // nil
    }

    void Entity::SetStorage(Tools::Lua::Ref const& storage)
    {
        if (this->_self.IsTable())
            this->_self.Set("storage", storage);
    }

}}}
