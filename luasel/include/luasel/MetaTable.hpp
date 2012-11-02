#ifndef __LUASEL_METATABLE_HPP__
#define __LUASEL_METATABLE_HPP__

#include <boost/lambda/construct.hpp>
#include <luasel/Interpreter.hpp>

namespace Luasel {

    namespace {
        template<class T>
            void _Destructor(T* a)
            {
                // Visual Studio doesn't want ~T() in lambdas...
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
                static MetaTable& Create(
                        Interpreter& interpreter,
                        std::function<Ref(void const*)> makeRef = std::function<Ref(void const*)>(),
                        std::function<void(Ref const&, void*)> makeNative = std::function<void(Ref const&, void*)>());
            template<class T>
                static MetaTable& Create(
                        Interpreter& interpreter, T&&,
                        std::function<Ref(void const*)> makeRef = std::function<Ref(void const*)>(),
                        std::function<void(Ref const&, void*)> makeNative = std::function<void(Ref const&, void*)>());
            template<class T>
                static MetaTable& Create(
                        Ref const& table, T&&,
                        std::function<Ref(void const*)> makeRef = std::function<Ref(void const*)>(),
                        std::function<void(Ref const&, void*)> makeNative = std::function<void(Ref const&, void*)>());

            MetaTable(MetaTable&& mt);

            template<class T>
                Ref MakeReference(T&& data) const;

            template<class T>
                typename std::enable_if<!std::is_pointer<T>::value, T>::type MakeNative(Ref const& ref) const;
            template<class T>
                typename std::enable_if<std::is_pointer<T>::value, T>::type MakeNative(Ref const& ref) const;

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

}

#endif
