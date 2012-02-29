#ifndef __TOOLS_LUA_METATABLE_HPP__
#define __TOOLS_LUA_METATABLE_HPP__

#include "tools/lua/Ref.hpp"
#include "tools/lua/Interpreter.hpp"

namespace Tools { namespace Lua {

    class MetaTable
    {
    public:
        enum MetaMethod
        {
            Add = 0,
            Substract,
            Multiply,
            Divide,
            Modulo,
            Power,
            UnaryMinus,
            Concatenate,
            Length,
            Equal,
            Less,
            LessEqual,
            Index,
            NewIndex,
            Call,
            Collect,
        };

    private:
        Interpreter& _interpreter;
        Ref _prototype;
        Ref _metaTable;

    public:
        template<class T>
        MetaTable(Interpreter& interpreter, T)
            : _interpreter(interpreter),
            _prototype(interpreter.MakeTable()),
            _metaTable(interpreter.MakeTable())
        {
            this->_metaTable.Set("__index", this->_prototype);
            this->_interpreter.GetState().RegisterMetaTable(this->_metaTable, typeid(T).hash_code());
        }

        MetaTable& AddMethod(std::string const& name, std::function<void(CallHelper&)> const& method);
        MetaTable& AddMetaMethod(MetaMethod type, std::function<void(CallHelper&)> const& method);
        MetaTable& AddMethod(std::string const& name, Ref const& method);
        MetaTable& AddMetaMethod(MetaMethod type, Ref const& method);

        Ref& GetPrototype() { return this->_prototype; }
        Ref& GetMetaTable() { return this->_metaTable; }
        Ref const& GetPrototype() const { return this->_prototype; }
        Ref const& GetMetaTable() const { return this->_metaTable; }
    };

}}

#endif
