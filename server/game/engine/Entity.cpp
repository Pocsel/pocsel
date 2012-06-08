#include "server/game/engine/Entity.hpp"
#include "server/game/engine/EntityType.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Iterator.hpp"

namespace Server { namespace Game { namespace Engine {

    Entity::Entity(Tools::Lua::Interpreter& interpreter, Uint32 id, EntityType* type) :
        _type(type), _self(interpreter.MakeTable())
    {
        this->_self.Set("id", id);
        this->_self.Set("storage", interpreter.MakeTable());
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
        if (this->_self.IsTable()) // toujours vrai en théorie, sauf si le moddeur a fait nimp avec self
            this->_self.Set("storage", storage);
    }

    void Entity::Disable(Tools::Lua::Interpreter& interpreter)
    {
        Tools::Lua::Ref idSave = interpreter.MakeNil();
        Tools::Lua::Ref storageSave = interpreter.MakeNil();
        if (this->_self.IsTable()) // toujours vrai en théorie, sauf si le moddeur fait nimp avec self
        {
            idSave = this->_self["id"];
            storageSave = this->_self["storage"];
        }
        this->_self = interpreter.MakeTable(); // perte de toutes les references/variables de l'instance
        this->_self.Set("id", idSave); // on garde qu'une table avec l'id
        this->_self.Set("storage", storageSave); // et le storage
    }

    void Entity::Enable(Tools::Lua::Interpreter& interpreter)
    {
        if (!this->_self.IsTable()) // toujours faux en théorie, sauf si le moddeur fait nimp avec self
            this->_self = interpreter.MakeTable(); // il n'y aura plus "id" & "storage" mais de toute maniere le moddeur ne les avait pas avant...
        Tools::Lua::Ref metatable = interpreter.MakeTable();
        this->_self.SetMetaTable(metatable);
        this->_self.Set("prototype", this->_type->GetPrototype());
        metatable.Set("__index", this->_type->GetPrototype());
    }

    void Entity::SaveToStorage()
    {
        if (!this->_self.IsTable())
            return;
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
        if (!this->_self.IsTable() || !this->_self["storage"].IsTable())
            return;
        auto it = this->_self["storage"].Begin();
        auto itEnd = this->_self["storage"].End();
        for (; it != itEnd; ++it)
            if (!it.GetKey().Equals("id") && !it.GetKey().Equals("prototype") && !it.GetKey().Equals("storage"))
                this->_self.Set(it.GetKey(), it.GetValue());
    }

}}}
