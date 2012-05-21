#include "server/game/engine/Entity.hpp"
#include "server/game/engine/EntityType.hpp"
#include "tools/lua/Interpreter.hpp"

namespace Server { namespace Game { namespace Engine {

    Entity::Entity(Tools::Lua::Interpreter& interpreter, Uint32 id, EntityType* type) :
        _type(type), _self(interpreter.MakeTable())
    {
        this->_self.Set("id", id);
        this->Enable(interpreter);
    }

    Entity::~Entity()
    {
    }

    Tools::Lua::Ref Entity::GetStorage() const
    {
        if (this->_self.IsTable()) // toujours vrai en théorie, sauf si le moddeur fait nimp avec self
            return this->_self["storage"];
        return Tools::Lua::Ref(this->_self.GetState()); // nil
    }

    void Entity::SetStorage(Tools::Lua::Ref const& storage)
    {
        if (this->_self.IsTable())
            this->_self.Set("storage", storage);
    }

    void Entity::Disable(Tools::Lua::Interpreter& interpreter)
    {
        Tools::Lua::Ref idSave = interpreter.MakeNil();
        if (this->_self.IsTable()) // toujours vrai en théorie, sauf si le moddeur fait nimp avec self
            idSave = this->_self["id"];
        this->_self = interpreter.MakeTable(); // perte de toutes les references/variables de l'instance
        this->_self.Set("id", idSave); // on garde qu'une table avec l'id
    }

    void Entity::Enable(Tools::Lua::Interpreter& interpreter)
    {
        if (!this->_self.IsTable()) // toujours faux en théorie, sauf si le moddeur fait nimp avec self
            this->_self = interpreter.MakeTable(); // il n'y aura plus "id" mais de toute maniere le moddeur ne l'avait pas avant...
        Tools::Lua::Ref metatable = interpreter.MakeTable();
        this->_self.SetMetaTable(metatable);
        this->_self.Set("prototype", this->_type->GetPrototype());
        this->_self.Set("storage", interpreter.MakeTable());
        metatable.Set("__index", this->_type->GetPrototype());
    }

}}}
