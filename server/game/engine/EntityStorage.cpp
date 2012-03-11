#include "server/game/engine/EntityStorage.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Ref.hpp"

namespace Server { namespace Game { namespace Engine {

    EntityStorage::EntityStorage(Tools::Lua::Interpreter& interpreter)
    {
        this->_table = new Tools::Lua::Ref(interpreter.MakeTable());
    }

    EntityStorage::~EntityStorage()
    {
        Tools::Delete(this->_table);
    }

    void EntityStorage::Length(Tools::Lua::CallHelper& helper)
    {
        helper.PushRet(helper.GetInterpreter().Make(this->_table->GetLength()));
    }

    void EntityStorage::Index(Tools::Lua::CallHelper& helper)
    {
        helper.PushRet(helper.GetInterpreter().GetSerializer().MakeSerializableCopy((*this->_table)[helper.PopArg()]));
    }

    void EntityStorage::NewIndex(Tools::Lua::CallHelper& helper)
    {
        auto index = helper.PopArg();
        if (!index.IsNumber() && !index.IsString() && !index.IsBoolean())
            throw std::runtime_error("Engine::EntityStorage: Storage is not indexable with " + index.GetTypeName() + " types");
        auto value = helper.GetInterpreter().GetSerializer().MakeSerializableCopy(helper.PopArg());
        this->_table->Set(index, value);
        // TODO ret?
    }

    void EntityStorage::Serialize(Tools::Lua::CallHelper& helper)
    {
        helper.PushRet(helper.GetInterpreter().MakeString("return " + helper.GetInterpreter().GetSerializer().Serialize(*this->_table)));
    }

}}}
