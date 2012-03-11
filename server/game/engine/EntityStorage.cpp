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
        helper.PushRet((*this->_table)[helper.PopArg()]);
    }

    void EntityStorage::NewIndex(Tools::Lua::CallHelper& helper)
    {
        auto index = helper.PopArg();
        auto value = helper.PopArg();
    }

    void EntityStorage::Serialize(Tools::Lua::CallHelper& helper)
    {
    }

}}}
