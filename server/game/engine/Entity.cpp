#include "server/game/engine/Entity.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/engine/EntityType.hpp"
#include "tools/lua/Interpreter.hpp"

namespace Server { namespace Game { namespace Engine {

    Entity::Entity(Engine& engine, Uint32 id, EntityType& type) :
        _type(type), _self(engine.GetInterpreter().MakeTable())
    {
        Tools::Lua::Ref metatable = engine.GetInterpreter().MakeTable();
        metatable.Set("__index", this->_type.GetPrototype());
        this->_self.SetMetaTable(metatable);
        this->_self.Set("id", id);
        this->_self.Set("prototype", this->_type.GetPrototype());
    }

    Entity::~Entity()
    {
    }

}}}
