#include <luasel/Interpreter.hpp>
#include <luasel/Lua.hpp>
#include <luasel/MetaTable.hpp>

namespace Luasel {

    namespace {
        static char const* const operators[] = {
            "__add",
            "__sub",
            "__mul",
            "__div",
            "__mod",
            "__pow",
            "__unm",
            "__concat",
            "__len",
            "__eq",
            "__lt",
            "__le",
            "__index",
            "__newindex",
            "__call",
            "__gc",
            "__serialize",
            "__clone",
        };
    }

    MetaTable& MetaTable::SetMethod(std::string const& name, std::function<void(CallHelper&)> const& method)
    {
        this->_prototype.Set(name, this->_interpreter.MakeFunction(method));
        return *this;
    }

    MetaTable& MetaTable::SetMetaMethod(MetaMethod type, std::function<void(CallHelper&)> const& method)
    {
        this->_metaTable.Set(operators[(int)type], this->_interpreter.MakeFunction(method));
        return *this;
    }

    MetaTable& MetaTable::SetMethod(std::string const& name, Ref const& method)
    {
        this->_prototype.Set(name, method);
        return *this;
    }

    MetaTable& MetaTable::SetMetaMethod(MetaMethod type, Ref const& method)
    {
        this->_metaTable.Set(operators[(int)type], method);
        return *this;
    }

    MetaTable::MetaTable(Interpreter& interpreter)
        : _interpreter(interpreter),
        _prototype(interpreter.MakeTable()),
        _metaTable(interpreter.MakeTable())
    {
        this->_metaTable.Set("__index", this->_prototype);
    }

    MetaTable::MetaTable(Ref const& metaTable)
        : _interpreter(metaTable.GetState().GetInterpreter()),
        _prototype(_interpreter.MakeTable()),
        _metaTable(metaTable)
    {
        this->_metaTable.Set("__index", this->_prototype);
    }

}
