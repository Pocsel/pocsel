#ifndef __TOOLS_LUA_METATABLE_HPP__
#define __TOOLS_LUA_METATABLE_HPP__

#include <boost/lambda/construct.hpp>

#include "tools/lua/Ref.hpp"
#include "tools/lua/Interpreter.hpp"

namespace Tools { namespace Lua {

    namespace {
        template<class T>
        void _Destructor(T* a)
        { // Parce que VS ne veut pas appeler "~T()" dans la lambda ><
            a->~T();
        };
    }

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
            this->SetMetaMethod(MetaTable::Collect, [](CallHelper& helper) { _Destructor(helper.PopArg().To<T*>()); });
            this->_interpreter.GetState().RegisterMetaTable(this->_metaTable, typeid(T).hash_code());
        }

        MetaTable& SetMethod(std::string const& name, std::function<void(CallHelper&)> const& method);
        MetaTable& SetMetaMethod(MetaMethod type, std::function<void(CallHelper&)> const& method);
        MetaTable& SetMethod(std::string const& name, Ref const& method);
        MetaTable& SetMetaMethod(MetaMethod type, Ref const& method);

        Ref& GetPrototype() { return this->_prototype; }
        Ref& GetMetaTable() { return this->_metaTable; }
        Ref const& GetPrototype() const { return this->_prototype; }
        Ref const& GetMetaTable() const { return this->_metaTable; }
    };

}}

#endif
