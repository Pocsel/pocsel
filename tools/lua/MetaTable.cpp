#include "tools/precompiled.hpp"

#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Lua.hpp"
#include "tools/lua/MetaTable.hpp"

namespace Tools { namespace Lua {

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
        };
    }

    MetaTable& MetaTable::AddMethod(std::string const& name, std::function<void(CallHelper&)> const& method)
    {
        this->_prototype.Set(name, this->_interpreter.MakeFunction(method));
        return *this;
    }

    MetaTable& MetaTable::AddMetaMethod(MetaMethod type, std::function<void(CallHelper&)> const& method)
    {
        this->_metaTable.Set(operators[(int)type], this->_interpreter.MakeFunction(method));
        return *this;
    }

    MetaTable& MetaTable::AddMethod(std::string const& name, Ref const& method)
    {
        this->_prototype.Set(name, method);
        return *this;
    }

    MetaTable& MetaTable::AddMetaMethod(MetaMethod type, Ref const& method)
    {
        this->_metaTable.Set(operators[(int)type], method);
        return *this;
    }

}}
