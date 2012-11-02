#ifndef __LUASEL_FUNCTION_HPP__
#define __LUASEL_FUNCTION_HPP__

#include <cassert>
#include <luasel/Iterator.hpp>
#include <luasel/Interpreter.hpp>
#include <luasel/Ref.hpp>
#include <luasel/MetaTable.hpp>
#include <luasel/Signature.hpp>

namespace Luasel {

    namespace {

        template<class T> struct _PointerWrapper { T* ptr; };
        template<class T> struct _GetLuaType { typedef T* type; };
        template<class T> struct _GetLuaType<T*> { typedef T* type; };
        template<class T> struct _GetLuaType<T&> : public _GetLuaType<T> {};
        template<class T> struct _GetLuaType<const T> : public _GetLuaType<T> {};
        template<> struct _GetLuaType<bool>          { typedef bool type; };
        template<> struct _GetLuaType<int>           { typedef int type; };
        template<> struct _GetLuaType<unsigned int>  { typedef unsigned int type; };
        template<> struct _GetLuaType<char>          { typedef char type; };
        template<> struct _GetLuaType<unsigned char> { typedef unsigned char type; };
        template<> struct _GetLuaType<float>         { typedef float type; };
        template<> struct _GetLuaType<double>        { typedef double type; };
        template<> struct _GetLuaType<std::string>   { typedef std::string type; };

        template<class T> struct _DeRef;
        template<class T> struct _DeRef<T*> { static T& Do(T* o) { return *o; } };
        template<class T> struct _DeRef<T*&> : public _DeRef<T*> {};
        template<class T> struct _DeRef<T&> { static T& Do(T& o) { return o; } };
        template<class T> struct _NoDeRef { static T Do(T o) { return o; } };
        template<> struct _DeRef<bool>          : public _NoDeRef<bool>          {};
        template<> struct _DeRef<int>           : public _NoDeRef<int>           {};
        template<> struct _DeRef<unsigned int>  : public _NoDeRef<unsigned int>  {};
        template<> struct _DeRef<char>          : public _NoDeRef<char>          {};
        template<> struct _DeRef<unsigned char> : public _NoDeRef<unsigned char> {};
        template<> struct _DeRef<float>         : public _NoDeRef<float>         {};
        template<> struct _DeRef<double>        : public _NoDeRef<double>        {};
        template<> struct _DeRef<std::string>   : public _NoDeRef<std::string>   {};

        template<class T, class U> struct _DeRef2 { static U Do(T const& o) { return o; } };
        template<class T, class U> struct _DeRef2<T*, U> { static U& Do(T* o) { return *o; } };
        template<class T, class U> struct _DeRef2<T*, U*> { static U* Do(T* o) { return o; } };

        template<class TRet, class TArgs>
            struct _Caller
            {
                private:
                    typedef typename TArgs::Head _Head;
                    typedef typename TArgs::Tail _Tail;

                public:
                    template<class TFunc>
                        static void Call(CallHelper& helper, TFunc func) { _Caller<TRet, _Tail>::Call(helper, func, helper.PopArg().Check<typename _GetLuaType<_Head>::type>()); }
                    template<class TFunc, class TArg1>
                        static void Call(CallHelper& helper, TFunc func, TArg1 p1) { _Caller<TRet, _Tail>::Call(helper, func, p1, helper.PopArg().Check<typename _GetLuaType<_Head>::type>()); }
                    template<class TFunc, class TArg1, class TArg2>
                        static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2) { _Caller<TRet, _Tail>::Call(helper, func, p1, p2, helper.PopArg().Check<typename _GetLuaType<_Head>::type>()); }
                    template<class TFunc, class TArg1, class TArg2, class TArg3>
                        static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2, TArg3 p3) { _Caller<TRet, _Tail>::Call(helper, func, p1, p2, p3, helper.PopArg().Check<typename _GetLuaType<_Head>::type>()); }
                    template<class TFunc, class TArg1, class TArg2, class TArg3, class TArg4>
                        static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2, TArg3 p3, TArg4 p4) { _Caller<TRet, _Tail>::Call(helper, func, p1, p2, p3, p4, helper.PopArg().Check<typename _GetLuaType<_Head>::type>()); }
            };

        template<class TRet>
            struct _Caller<TRet, Luasel::NullList>
            {
                template<class TFunc>
                    static void Call(CallHelper& helper, TFunc func) { helper.PushRet(helper.GetInterpreter().Make(func())); }
                template<class TFunc, class TArg1>
                    static void Call(CallHelper& helper, TFunc func, TArg1 p1) { helper.PushRet(helper.GetInterpreter().Make(func(_DeRef2<TArg1, typename TFunc::argument_type>::Do(p1)))); }
                template<class TFunc, class TArg1, class TArg2>
                    static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2) { helper.PushRet(helper.GetInterpreter().Make(func(_DeRef2<TArg1, typename TFunc::first_argument_type>::Do(p1), _DeRef<TArg2>::Do(p2)))); }
                template<class TFunc, class TArg1, class TArg2, class TArg3>
                    static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2, TArg3 p3) { helper.PushRet(helper.GetInterpreter().Make(func(_DeRef2<TArg1, typename TFunc::first_argument_type>::Do(p1), _DeRef<TArg2>::Do(p2), _DeRef<TArg3>::Do(p3)))); }
                template<class TFunc, class TArg1, class TArg2, class TArg3, class TArg4>
                    static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2, TArg3 p3, TArg4 p4) { helper.PushRet(helper.GetInterpreter().Make(func(_DeRef2<TArg1, typename TFunc::first_argument_type>::Do(p1), _DeRef<TArg2>::Do(p2), _DeRef<TArg3>::Do(p3), _DeRef<TArg4>::Do(p4)))); }
                template<class TFunc, class TArg1, class TArg2, class TArg3, class TArg4, class TArg5>
                    static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2, TArg3 p3, TArg4 p4, TArg5 p5) { helper.PushRet(helper.GetInterpreter().Make(func(_DeRef2<TArg1, typename TFunc::first_argument_type>::Do(p1), _DeRef<TArg2>::Do(p2), _DeRef<TArg3>::Do(p3), _DeRef<TArg4>::Do(p4), _DeRef<TArg5>::Do(p5)))); }
            };

        template<class TRet>
            struct _Caller<TRet*, Luasel::NullList>
            {
                template<class TFunc>
                    static void Call(CallHelper& helper, TFunc func) { helper.PushRet(helper.GetInterpreter().Make(func())); }
                template<class TFunc, class TArg1>
                    static void Call(CallHelper& helper, TFunc func, TArg1 p1) { helper.PushRet(helper.GetInterpreter().Make(func(_DeRef2<TArg1, typename TFunc::argument_type>::Do(p1)))); }
                template<class TFunc, class TArg1, class TArg2>
                    static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2) { helper.PushRet(helper.GetInterpreter().Make(func(_DeRef2<TArg1, typename TFunc::first_argument_type>::Do(p1), _DeRef<TArg2>::Do(p2)))); }
                template<class TFunc, class TArg1, class TArg2, class TArg3>
                    static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2, TArg3 p3) { helper.PushRet(helper.GetInterpreter().Make(func(_DeRef2<TArg1, typename TFunc::first_argument_type>::Do(p1), _DeRef<TArg2>::Do(p2), _DeRef<TArg3>::Do(p3)))); }
                template<class TFunc, class TArg1, class TArg2, class TArg3, class TArg4>
                    static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2, TArg3 p3, TArg4 p4) { helper.PushRet(helper.GetInterpreter().Make(func(_DeRef2<TArg1, typename TFunc::first_argument_type>::Do(p1), _DeRef<TArg2>::Do(p2), _DeRef<TArg3>::Do(p3), _DeRef<TArg4>::Do(p4)))); }
                template<class TFunc, class TArg1, class TArg2, class TArg3, class TArg4, class TArg5>
                    static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2, TArg3 p3, TArg4 p4, TArg5 p5) { helper.PushRet(helper.GetInterpreter().Make(func(_DeRef2<TArg1, typename TFunc::first_argument_type>::Do(p1), _DeRef<TArg2>::Do(p2), _DeRef<TArg3>::Do(p3), _DeRef<TArg4>::Do(p4), _DeRef<TArg5>::Do(p5)))); }
            };

        template<>
            struct _Caller<void, Luasel::NullList>
            {
                template<class TFunc>
                    static void Call(CallHelper&, TFunc func) { func(); }
                template<class TFunc, class TArg1>
                    static void Call(CallHelper&, TFunc func, TArg1 p1) { func(_DeRef2<TArg1, typename TFunc::argument_type>::Do(p1)); }
                template<class TFunc, class TArg1, class TArg2>
                    static void Call(CallHelper&, TFunc func, TArg1 p1, TArg2 p2) { func(_DeRef2<TArg1, typename TFunc::first_argument_type>::Do(p1), _DeRef<TArg2>::Do(p2)); }
                template<class TFunc, class TArg1, class TArg2, class TArg3>
                    static void Call(CallHelper&, TFunc func, TArg1 p1, TArg2 p2, TArg3 p3) { func(_DeRef2<TArg1, typename TFunc::first_argument_type>::Do(p1), _DeRef<TArg2>::Do(p2), _DeRef<TArg3>::Do(p3)); }
                template<class TFunc, class TArg1, class TArg2, class TArg3, class TArg4>
                    static void Call(CallHelper&, TFunc func, TArg1 p1, TArg2 p2, TArg3 p3, TArg4 p4) { func(_DeRef2<TArg1, typename TFunc::first_argument_type>::Do(p1), _DeRef<TArg2>::Do(p2), _DeRef<TArg3>::Do(p3), _DeRef<TArg4>::Do(p4)); }
                template<class TFunc, class TArg1, class TArg2, class TArg3, class TArg4, class TArg5>
                    static void Call(CallHelper&, TFunc func, TArg1 p1, TArg2 p2, TArg3 p3, TArg4 p4, TArg5 p5) { func(_DeRef2<TArg1, typename TFunc::first_argument_type>::Do(p1), _DeRef<TArg2>::Do(p2), _DeRef<TArg3>::Do(p3), _DeRef<TArg4>::Do(p4), _DeRef<TArg5>::Do(p5)); }
            };

        template<>
            struct _Caller<void, Luasel::TypeList<CallHelper&, Luasel::NullList>>
            {
                template<class TFunc>
                    static void Call(CallHelper& helper, TFunc func) { func(helper); }
            };

        template<class Signature>
            class _Functor
            {
                private:
                    typename Signature::Prototype _function;

                public:
                    _Functor(typename Signature::Prototype function)
                        : _function(function)
                    {
                    }

                    void operator ()(CallHelper& helper)
                    {
                        _Caller<typename Signature::ReturnType, typename Signature::Parameters>::Call(helper, this->_function);
                    }
            };
    }

    /********* Interpreter *********/
    template <typename T>
        inline Ref Interpreter::Make(T val) throw(std::runtime_error)
        {
            auto const& m = this->_state->GetMetaTable(typeid(T).hash_code());
            return m.MakeReference(std::move(val));
        }
    template<> inline Ref Interpreter::Make<bool>(bool val) throw(std::runtime_error) { return this->_state->Make(val); }
    template<> inline Ref Interpreter::Make<int>(int val) throw(std::runtime_error) { return this->_state->Make(val); }
    template<> inline Ref Interpreter::Make<unsigned int>(unsigned int val) throw(std::runtime_error) { return this->_state->Make(val); }
    template<> inline Ref Interpreter::Make<char>(char val) throw(std::runtime_error) { return this->_state->Make(val); }
    template<> inline Ref Interpreter::Make<unsigned char>(unsigned char val) throw(std::runtime_error) { return this->_state->Make(val); }
    template<> inline Ref Interpreter::Make<double>(double val) throw(std::runtime_error) { return this->_state->Make(val); }
    template<> inline Ref Interpreter::Make<float>(float val) throw(std::runtime_error) { return this->_state->Make(val); }
    template<> inline Ref Interpreter::Make<std::string>(std::string val) throw(std::runtime_error) { return this->_state->Make(val); }
    template<> inline Ref Interpreter::Make<char const*>(char const* val) throw(std::runtime_error) { return this->_state->Make(val); }
    template<> inline Ref Interpreter::Make<std::function<void(CallHelper&)>>(std::function<void(CallHelper&)> val) throw(std::runtime_error) { return this->_state->Make(val); }
    template<> inline Ref Interpreter::Make<Ref>(Ref val) throw(std::runtime_error) { return this->_state->Make(val); }
    template <typename T>
        inline Ref Interpreter::MakeMove(T&& val) throw(std::runtime_error)
        {
            auto const& m = this->_state->GetMetaTable(typeid(T).hash_code());
            return m.MakeReference(std::move(val));
        }

    template<class T>
        Ref Interpreter::Bind(T function)
        {
            return this->MakeFunction(_Functor<Luasel::Signature<T>>(std::bind(function)));
        }

    template<class T, class TA1>
        Ref Interpreter::Bind(T function, TA1 arg1)
        {
            return this->MakeFunction(_Functor<Luasel::Signature<T>>(std::bind(function, arg1)));
        }

    template<class T, class TA1, class TA2>
        Ref Interpreter::Bind(T function, TA1 arg1, TA2 arg2)
        {
            return this->MakeFunction(_Functor<Luasel::Signature<T>>(std::bind(function, arg1, arg2)));
        }

    template<class T, class TA1, class TA2, class TA3>
        Ref Interpreter::Bind(T function, TA1 arg1, TA2 arg2, TA3 arg3)
        {
            return this->MakeFunction(_Functor<Luasel::Signature<T>>(std::bind(function, arg1, arg2, arg3)));
        }

    /********* Ref *********/
    template<class T>
        inline T Ref::To() const throw()
        {
            auto& cppMt = this->_state.GetMetaTable(typeid(typename std::remove_pointer<T>::type).hash_code());
            return cppMt.MakeNative<T>(*this);
        }
    template<> bool Ref::To<bool>() const throw();
    template<> int Ref::To<int>() const throw();
    template<> unsigned int Ref::To<unsigned int>() const throw();
    template<> char Ref::To<char>() const throw();
    template<> unsigned char Ref::To<unsigned char>() const throw();
    template<> double Ref::To<double>() const throw();
    template<> float Ref::To<float>() const throw();
    template<> std::string Ref::To<std::string>() const throw();
    template<> inline Ref Ref::To<Ref>() const throw() { return *this; }

    template<class T>
        inline bool Ref::Is() const throw()
        {
            if (this->GetMetaTable().IsNoneOrNil() ||
                    this->GetMetaTable() != this->_state.GetMetaTable(typeid(typename std::remove_pointer<T>::type).hash_code()).GetMetaTable())
                return false;
            return true;
        }

    template<> bool Ref::Check<bool>(std::string const& e /* = "" */) const throw(std::runtime_error);
    template<> int Ref::Check<int>(std::string const& e /* = "" */) const throw(std::runtime_error);
    template<> unsigned int Ref::Check<unsigned int>(std::string const& e /* = "" */) const throw(std::runtime_error);
    template<> char Ref::Check<char>(std::string const& e /* = "" */) const throw(std::runtime_error);
    template<> unsigned char Ref::Check<unsigned char>(std::string const& e /* = "" */) const throw(std::runtime_error);
    template<> double Ref::Check<double>(std::string const& e /* = "" */) const throw(std::runtime_error);
    template<> float Ref::Check<float>(std::string const& e /* = "" */) const throw(std::runtime_error);
    template<> std::string Ref::Check<std::string>(std::string const& e /* = "" */) const throw(std::runtime_error);
    template<> inline Ref Ref::Check<Ref>(std::string const&) const throw(std::runtime_error) { return *this; }

    template<class T>
        inline T Ref::Check(std::string const& err /* = "" */) const throw(std::runtime_error)
        {
            try
            {
                auto mt = this->GetMetaTable();
                if (mt.IsNoneOrNil())
                    throw 1;
                auto& cppMt = this->_state.GetMetaTable(typeid(typename std::remove_pointer<T>::type).hash_code());
                if (mt != cppMt.GetMetaTable())
                    throw 1;

                return cppMt.MakeNative<T>(*this);
            }
            catch (int)
            {
                if (!err.empty())
                    throw std::runtime_error(err);
                else
                    throw std::runtime_error(std::string("Luasel::Ref: Value is not of \"") + typeid(typename std::remove_pointer<T>::type).name() + "\" type");
            }
            catch (std::exception& e)
            {
                if (!err.empty())
                    throw std::runtime_error(err);
                else
                    throw std::runtime_error(std::string("Luasel::Ref::Check<") + typeid(typename std::remove_pointer<T>::type).name() + ">: " + e.what());
            }
        }

    template <typename T>
        inline T Ref::To(T const& defaultValue) const throw()
        {
            try
            {
                return this->Check<T>();
            }
            catch (std::exception& e)
            {
                return defaultValue;
            }
        }

    template <typename T>
        inline Ref Ref::operator ()(T a1) const throw(std::runtime_error)
        {
            CallHelper callHelper(this->_state.GetInterpreter());
            callHelper.PushArg(this->_state.GetInterpreter().Make(a1));
            this->Call(callHelper);
            if (callHelper.GetNbRets())
                return callHelper.PopRet();
            return Ref(*this);
        }

    template <typename T, typename U>
        inline Ref Ref::operator ()(T a1, U a2) const throw(std::runtime_error)
        {
            CallHelper callHelper(this->_state.GetInterpreter());
            callHelper.PushArg(this->_state.GetInterpreter().Make(a1));
            callHelper.PushArg(this->_state.GetInterpreter().Make(a2));
            this->Call(callHelper);
            if (callHelper.GetNbRets())
                return callHelper.PopRet();
            return Ref(*this);
        }

    template <typename T, typename U, typename V>
        inline Ref Ref::operator ()(T a1, U a2, V a3) const throw(std::runtime_error)
        {
            CallHelper callHelper(this->_state.GetInterpreter());
            callHelper.PushArg(this->_state.GetInterpreter().Make(a1));
            callHelper.PushArg(this->_state.GetInterpreter().Make(a2));
            callHelper.PushArg(this->_state.GetInterpreter().Make(a3));
            this->Call(callHelper);
            if (callHelper.GetNbRets())
                return callHelper.PopRet();
            return Ref(*this);
        }

    template <typename T, typename U, typename V, typename W>
        inline Ref Ref::operator ()(T a1, U a2, V a3, W a4) const throw(std::runtime_error)
        {
            CallHelper callHelper(this->_state.GetInterpreter());
            callHelper.PushArg(this->_state.GetInterpreter().Make(a1));
            callHelper.PushArg(this->_state.GetInterpreter().Make(a2));
            callHelper.PushArg(this->_state.GetInterpreter().Make(a3));
            callHelper.PushArg(this->_state.GetInterpreter().Make(a4));
            this->Call(callHelper);
            if (callHelper.GetNbRets())
                return callHelper.PopRet();
            return Ref(*this);
        }

    template <typename T>
        inline Ref Ref::operator [](T index) const throw(std::runtime_error)
        {
            return (*this)[this->_state.Make(index)];
        }

    template <typename T, typename U>
        inline Ref Ref::Set(T key, U value) const throw(std::runtime_error)
        {
            return this->Set(this->_state.Make(key), this->_state.Make(value));
        }

    template <typename T>
        inline bool Ref::Equals(T value) const throw()
        {
            return *this == this->_state.Make(value);
        }

    /********* CallHelper *********/
    // arguments helpers
    template<class T>
        inline void CallHelper::PushArg(T const& arg) throw()
        {
            this->PushArg(this->_i.Make(arg));
        }
    template<class T>
        inline void CallHelper::PushArgMove(T&& arg) throw()
        {
            this->PushArg(this->_i.Make(std::move(arg)));
        }

    // return helpers
    template<class T>
        inline void CallHelper::PushRet(T const& ret) throw()
        {
            this->PushRet(this->_i.Make(ret));
        }
    template<class T>
        inline void CallHelper::PushRetMove(T&& ret) throw()
        {
            this->PushRet(this->_i.Make(std::move(ret)));
        }

    /********* MetaTable *********/
    template<class T>
        inline MetaTable& MetaTable::Create(
                Interpreter& interpreter,
                std::function<Ref(void const*)> makeRef,
                std::function<void(Ref const&, void*)> makeNative)
        {
            auto& tmp = interpreter.GetState().RegisterMetaTable(MetaTable(interpreter), typeid(T).hash_code());;
            tmp._makeRef = makeRef;
            tmp._makeNative = makeNative;
            tmp._metaTable.Set("__index", tmp._prototype);
            tmp.SetMetaMethod(MetaTable::Collect, [](CallHelper& helper) { _Destructor(helper.PopArg().To<T*>()); });
            return tmp;
        }
    template<class T>
        inline MetaTable& MetaTable::Create(
                Interpreter& interpreter, T&&,
                std::function<Ref(void const*)> makeRef,
                std::function<void(Ref const&, void*)> makeNative)
        {
            auto& tmp = interpreter.GetState().RegisterMetaTable(MetaTable(interpreter), typeid(T).hash_code());;
            tmp._makeRef = makeRef;
            tmp._makeNative = makeNative;
            tmp.SetMetaMethod(MetaTable::Collect, [](CallHelper& helper) { _Destructor(helper.PopArg().To<T*>()); });
            return tmp;
        }
    template<class T>
        inline MetaTable& MetaTable::Create(
                Ref const& table, T&&,
                std::function<Ref(void const*)> makeRef,
                std::function<void(Ref const&, void*)> makeNative)
        {
            auto& tmp = table.GetState().RegisterMetaTable(MetaTable(table), typeid(T).hash_code());;
            tmp._makeRef = makeRef;
            tmp._makeNative = makeNative;
            tmp.SetMetaMethod(MetaTable::Collect, [](CallHelper& helper) { _Destructor(helper.PopArg().To<T*>()); });
            return tmp;
        }

    inline MetaTable::MetaTable(MetaTable&& mt) :
        _interpreter(mt._interpreter),
        _prototype(std::move(mt._prototype)),
        _metaTable(std::move(mt._metaTable)),
        _makeRef(std::move(mt._makeRef))
    {
    }

    template<class T>
        inline Ref MetaTable::MakeReference(T&& data) const
        {
            if (this->_makeRef)
                return this->_makeRef(&data);
            T* luaValue = 0;
            auto r = this->_interpreter.MakeUserData(reinterpret_cast<void**>(&luaValue), sizeof(T));
            new (luaValue) T(std::move(data));
            r.SetMetaTable(this->_metaTable);
            return r;
        }

    template<class T>
        inline typename std::enable_if<!std::is_pointer<T>::value, T>::type MetaTable::MakeNative(Ref const& ref) const
        {
            if (this->_makeNative)
            {
                T value;
                this->_makeNative(ref, &value);
                return value;
            }
            assert("Do not use Check<Type> but Check<Type*>, or specify a \"MakeNative\" function");
            throw std::runtime_error("Do not use Check<Type> but Check<Type*>, or specify a \"MakeNative\" function");
        }
    template<class T>
        inline typename std::enable_if<std::is_pointer<T>::value, T>::type MetaTable::MakeNative(Ref const& ref) const
        {
            return reinterpret_cast<T>(ref.CheckUserData());
        }

}

#endif
