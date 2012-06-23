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

    class MetaTable : private boost::noncopyable
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
            Serialize,
            Clone,
        };

    private:
        Interpreter& _interpreter;
        Ref _prototype;
        Ref _metaTable;
        std::function<Ref(void const*)> _makeRef;
        std::function<void(Ref const&, void*)> _makeNative;

    public:
        template<class T>
        static MetaTable& Create(Interpreter& interpreter,
            std::function<Ref(void const*)> makeRef = std::function<Ref(void const*)>(),
            std::function<void(Ref const&, void*)> makeNative = std::function<void(Ref const&, void*)>())
        {
            auto& tmp = interpreter.GetState().RegisterMetaTable(MetaTable(interpreter), typeid(T).hash_code());;
            tmp._makeRef = makeRef;
            tmp._makeNative = makeNative;
            tmp._metaTable.Set("__index", tmp._prototype);
            tmp.SetMetaMethod(MetaTable::Collect, [](CallHelper& helper) { _Destructor(helper.PopArg().To<T*>()); });
            return tmp;
        }
        template<class T>
        static MetaTable& Create(Interpreter& interpreter, T&&,
            std::function<Ref(void const*)> makeRef = std::function<Ref(void const*)>(),
            std::function<void(Ref const&, void*)> makeNative = std::function<void(Ref const&, void*)>())
        {
            auto& tmp = interpreter.GetState().RegisterMetaTable(MetaTable(interpreter), typeid(T).hash_code());;
            tmp._makeRef = makeRef;
            tmp._makeNative = makeNative;
            tmp.SetMetaMethod(MetaTable::Collect, [](CallHelper& helper) { _Destructor(helper.PopArg().To<T*>()); });
            return tmp;
        }
        template<class T>
        static MetaTable& Create(Ref const& table, T&&,
            std::function<Ref(void const*)> makeRef = std::function<Ref(void const*)>(),
            std::function<void(Ref const&, void*)> makeNative = std::function<void(Ref const&, void*)>())
        {
            auto& tmp = table.GetState().RegisterMetaTable(MetaTable(table), typeid(T).hash_code());;
            tmp._makeRef = makeRef;
            tmp._makeNative = makeNative;
            tmp.SetMetaMethod(MetaTable::Collect, [](CallHelper& helper) { _Destructor(helper.PopArg().To<T*>()); });
            return tmp;
        }

        MetaTable(MetaTable&& mt) :
            _interpreter(mt._interpreter),
            _prototype(std::move(mt._prototype)),
            _metaTable(std::move(mt._metaTable)),
            _makeRef(std::move(mt._makeRef))
        {
        }

#ifdef new
# undef new
#endif

        template<class T>
        Ref MakeReference(T&& data) const
        {
            if (this->_makeRef)
                return this->_makeRef(&data);
            T* luaValue = 0;
            auto r = this->_interpreter.MakeUserData(reinterpret_cast<void**>(&luaValue), sizeof(T));
            new (luaValue) T(std::move(data));
            r.SetMetaTable(this->_metaTable);
            return r;
        }

#ifdef DEBUG_NEW
# define new DEBUG_NEW
#endif

        template<class T>
        typename std::enable_if<!std::is_pointer<T>::value, T>::type MakeNative(Ref const& ref) const
        {
            if (this->_makeNative)
            {
                T value;
                this->_makeNative(ref, &value);
                return value;
            }
            assert("Pas possible, ne pas faire Check<Type> mais Check<Type*> ou specifier un \"makeNative\".");
            throw std::runtime_error("Call Martin !");
        }
        template<class T>
        typename std::enable_if<std::is_pointer<T>::value, T>::type MakeNative(Ref const& ref) const
        {
            return reinterpret_cast<T>(ref.CheckUserData());
        }

        MetaTable& SetMethod(std::string const& name, std::function<void(CallHelper&)> const& method);
        MetaTable& SetMetaMethod(MetaMethod type, std::function<void(CallHelper&)> const& method);
        MetaTable& SetMethod(std::string const& name, Ref const& method);
        MetaTable& SetMetaMethod(MetaMethod type, Ref const& method);

        Ref& GetPrototype() { return this->_prototype; }
        Ref& GetMetaTable() { return this->_metaTable; }
        Ref const& GetPrototype() const { return this->_prototype; }
        Ref const& GetMetaTable() const { return this->_metaTable; }

    private:
        MetaTable(Interpreter& interpreter);
        MetaTable(Ref const& metaTable);
    };

}}

#endif
