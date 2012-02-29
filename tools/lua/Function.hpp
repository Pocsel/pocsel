#ifndef __TOOLS_LUA_FUNCTION_HPP__
#define __TOOLS_LUA_FUNCTION_HPP__

#include "tools/precompiled.hpp"

#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Ref.hpp"

#include "tools/meta/Signature.hpp"

namespace Tools { namespace Lua {

    namespace {

        template<class T> struct _GetLuaType { typedef T* type; };
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

        template<class T> struct _DeRef { static T Do(T* o) { return *o; } };
        template<class T> struct _DeRef<T&> : public _DeRef<T> {};
        template<class T> struct _NoDeRef { static T Do(T o) { return o; } };
        template<> struct _DeRef<bool>          : public _NoDeRef<bool>          {};
        template<> struct _DeRef<int>           : public _NoDeRef<int>           {};
        template<> struct _DeRef<unsigned int>  : public _NoDeRef<unsigned int>  {};
        template<> struct _DeRef<char>          : public _NoDeRef<char>          {};
        template<> struct _DeRef<unsigned char> : public _NoDeRef<unsigned char> {};
        template<> struct _DeRef<float>         : public _NoDeRef<float>         {};
        template<> struct _DeRef<double>        : public _NoDeRef<double>        {};
        template<> struct _DeRef<std::string>   : public _NoDeRef<std::string>   {};

        template<class T, class U> struct _DeRef2 { static T Do(T o) { return o; } };
        template<class T, class U> struct _DeRef2<T*, U> { static T Do(T* o) { return *o; } };
        template<class T, class U> struct _DeRef2<T, U*> { static T* Do(T o) { return &o; } };

        template<class TRet, class TArgs>
        struct _Caller
        {
        private:
            typedef typename TArgs::Head _Head;
            typedef typename TArgs::Tail _Tail;

        public:
            template<class TFunc>
            static void Call(CallHelper& helper, TFunc func) { _Caller<TRet, _Tail>::Call(helper, func, _DeRef<_Head>::Do(helper.PopArg().Check<_GetLuaType<_Head>::type>())); }
            template<class TFunc, class TArg1>
            static void Call(CallHelper& helper, TFunc func, TArg1 p1) { _Caller<TRet, _Tail>::Call(helper, func, p1, helper.PopArg().Check<_GetLuaType<_Head>::type>()); }
            template<class TFunc, class TArg1, class TArg2>
            static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2) { _Caller<TRet, _Tail>::Call(helper, func, p1, p2, helper.PopArg().Check<_GetLuaType<_Head>::type>()); }
            template<class TFunc, class TArg1, class TArg2, class TArg3>
            static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2, TArg3 p3) { _Caller<TRet, _Tail>::Call(helper, func, p1, p2, p3, helper.PopArg().Check<_GetLuaType<_Head>::type>()); }
            template<class TFunc, class TArg1, class TArg2, class TArg3, class TArg4>
            static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2, TArg3 p3, TArg4 p4) { _Caller<TRet, _Tail>::Call(helper, func, p1, p2, p3, p4, helper.PopArg().Check<_GetLuaType<_Head>::type>()); }
        };

        template<class TRet>
        struct _Caller<TRet, Tools::Meta::NullList>
        {
            template<class TFunc>
            static void Call(CallHelper& helper, TFunc func) { helper.PushRet(helper.GetInterpreter().Make(func())); }
            template<class TFunc, class TArg1>
            static void Call(CallHelper& helper, TFunc func, TArg1 p1) { helper.PushRet(helper.GetInterpreter().Make(func(p1))); }
            template<class TFunc, class TArg1, class TArg2>
            static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2) { helper.PushRet(helper.GetInterpreter().Make(func(p1, p2))); }
            template<class TFunc, class TArg1, class TArg2, class TArg3>
            static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2, TArg3 p3) { helper.PushRet(helper.GetInterpreter().Make(func(p1, p2, p3))); }
            template<class TFunc, class TArg1, class TArg2, class TArg3, class TArg4>
            static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2, TArg3 p3, TArg4 p4) { helper.PushRet(helper.GetInterpreter().Make(func(p1, p2, p3, p4))); }
            template<class TFunc, class TArg1, class TArg2, class TArg3, class TArg4, class TArg5>
            static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2, TArg3 p3, TArg4 p4, TArg5 p5) { helper.PushRet(helper.GetInterpreter().Make(func(p1, p2, p3, p4, p5))); }
        };

        template<class TRet>
        struct _Caller<TRet*, Tools::Meta::NullList>
        {
            template<class TFunc>
            static void Call(CallHelper& helper, TFunc func) { helper.PushRet(helper.GetInterpreter().Make(func())); }
            template<class TFunc, class TArg1>
            static void Call(CallHelper& helper, TFunc func, TArg1 p1) { helper.PushRet(helper.GetInterpreter().Make(func(p1))); }
            template<class TFunc, class TArg1, class TArg2>
            static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2) { helper.PushRet(helper.GetInterpreter().Make(func(p1, p2))); }
            template<class TFunc, class TArg1, class TArg2, class TArg3>
            static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2, TArg3 p3) { helper.PushRet(helper.GetInterpreter().Make(func(p1, p2, p3))); }
            template<class TFunc, class TArg1, class TArg2, class TArg3, class TArg4>
            static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2, TArg3 p3, TArg4 p4) { helper.PushRet(helper.GetInterpreter().Make(func(p1, p2, p3, p4))); }
            template<class TFunc, class TArg1, class TArg2, class TArg3, class TArg4, class TArg5>
            static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2, TArg3 p3, TArg4 p4, TArg5 p5) { helper.PushRet(helper.GetInterpreter().Make(func(p1, p2, p3, p4, p5))); }
        };

        template<>
        struct _Caller<void, Tools::Meta::NullList>
        {
            template<class TFunc>
            static void Call(CallHelper& helper, TFunc func) { func(); }
            template<class TFunc, class TArg1>
            static void Call(CallHelper& helper, TFunc func, TArg1 p1) { func(_DeRef2<TArg1, typename TFunc::argument_type>::Do(p1)); }
            template<class TFunc, class TArg1, class TArg2>
            static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2) { func(_DeRef2<TArg1, typename TFunc::first_argument_type>::Do(p1), _DeRef2<TArg2, typename TFunc::second_argument_type>::Do(p2)); }
            //template<class TFunc, class TArg1, class TArg2, class TArg3>
            //static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2, TArg3 p3) { func(_DeRef2<TArg1, typename TFunc::first_argument_type>::Do(p1), _DeRef<TArg2>::Do(p2), _DeRef<TArg3>::Do(p3)); }
            //template<class TFunc, class TArg1, class TArg2, class TArg3, class TArg4>
            //static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2, TArg3 p3, TArg4 p4) { func(_DeRef<TArg1>::Do(p1), _DeRef<TArg2>::Do(p2), _DeRef<TArg3>::Do(p3), _DeRef<TArg4>::Do(p4)); }
            //template<class TFunc, class TArg1, class TArg2, class TArg3, class TArg4, class TArg5>
            //static void Call(CallHelper& helper, TFunc func, TArg1 p1, TArg2 p2, TArg3 p3, TArg4 p4, TArg5 p5) { func(_DeRef<TArg1>::Do(p1), _DeRef<TArg2>::Do(p2), _DeRef<TArg3>::Do(p3), _DeRef<TArg4>::Do(p4), _DeRef<TArg5>::Do(p5)); }
        };

        template<>
        struct _Caller<void, Tools::Meta::TypeList<CallHelper&, Tools::Meta::NullList>>
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

    template<class T>
    Ref Interpreter::Bind(T function)
    {
        return this->MakeFunction(_Functor<Tools::Meta::Signature<T>>(function));
    }

    template<class T, class TA1>
    Ref Interpreter::Bind(T function, TA1 arg1)
    {
        return this->MakeFunction(_Functor<Tools::Meta::Signature<T>>(std::bind(function, arg1)));
    }

    template<class T, class TA1, class TA2>
    Ref Interpreter::Bind(T function, TA1 arg1, TA2 arg2)
    {
        return this->MakeFunction(_Functor<Tools::Meta::Signature<T>>(std::bind(function, arg1, arg2)));
    }

    template<class T, class TA1, class TA2, class TA3>
    Ref Interpreter::Bind(T function, TA1 arg1, TA2 arg2, TA3 arg3)
    {
        return this->MakeFunction(_Functor<Tools::Meta::Signature<T>>(std::bind(function, arg1, arg2, arg3)));
    }

}}

#endif