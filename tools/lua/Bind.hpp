#ifndef __TOOLS_LUA_BIND_HPP__
#define __TOOLS_LUA_BIND_HPP__

#include "tools/meta/Signature.hpp"
#include "tools/meta/RemoveReference.hpp"
#include "tools/meta/RemoveQualifiers.hpp"

#include "Interpreter.hpp"
#include "LuaState.hpp"
#include "StackRef.hpp"

namespace Tools { namespace Lua {

    template<typename Ret, typename Params> struct Caller
    {
        typedef typename Params::Head Head;
        typedef typename Params::Tail Tail;
        template<typename T> static void Call(LuaState& api, T f)
        {
            Head h = api.to<Head>(-1);
            api.pop(1);
            Caller<Ret, Tail>::Call(api, f, h);
        }
        template<typename T, typename P1> static void Call(LuaState& api, T f, P1 p1)
        {
            Head h = api.to<Head>(-1);
            api.pop(1);
            Caller<Ret, Tail>::Call(api, f, h, p1);
        }
        template<typename T, typename P1, typename P2> static void Call(LuaState& api, T f, P1 p1, P2 p2)
        {
            Head h = api.to<Head>(-1);
            api.pop(1);
            Caller<Ret, Tail>::Call(api, f, h, p1, p2);
        }
        template<typename T, typename P1, typename P2, typename P3> static void Call(LuaState& api, T f, P1 p1, P2 p2, P3 p3)
        {
            Head h = api.to<Head>(-1);
            api.pop(1);
            Caller<Ret, Tail>::Call(api, f, h, p1, p2, p3);
        }
        template<typename T, typename P1, typename P2, typename P3, typename P4> static void Call(LuaState& api, T f, P1 p1, P2 p2, P3 p3, P4 p4)
        {
            Head h = api.to<Head>(-1);
            api.pop(1);
            Caller<Ret, Tail>::Call(api, f, h, p1, p2, p3, p4);
        }
    };

    template<typename Ret> struct Caller<Ret, TOOLS_META_TYPELIST0()>
    {
        template<typename T> static void Call(LuaState& api, T f)
        { api.push<Ret>(f()); }
        template<typename T, typename P1> static void Call(LuaState& api, T f, P1 p1)
        { api.push<Ret>(f(p1)); }
        template<typename T, typename P1, typename P2> static void Call(LuaState& api, T f, P1 p1, P2 p2)
        { api.push<Ret>(f(p1, p2)); }
        template<typename T, typename P1, typename P2, typename P3> static void Call(LuaState& api, T f, P1 p1, P2 p2, P3 p3)
        { api.push<Ret>(f(p1, p2, p3)); }
        template<typename T, typename P1, typename P2, typename P3, typename P4> static void Call(LuaState& api, T f, P1 p1, P2 p2, P3 p3, P4 p4)
        { api.push<Ret>(f(p1, p2, p3, p4)); }
        template<typename T, typename P1, typename P2, typename P3, typename P4, typename P5> static void Call(LuaState& api, T f, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
        { api.push<Ret>(f(p1, p2, p3, p4, p5)); }
    };

    template<> struct Caller<void, TOOLS_META_TYPELIST0()>
    {
        template<typename T> static void Call(LuaState&, T f)
        { f(); }
        template<typename T, typename P1> static void Call(LuaState&, T f, P1 p1)
        { f(p1); }
        template<typename T, typename P1, typename P2> static void Call(LuaState&, T f, P1 p1, P2 p2)
        { f(p1, p2); }
        template<typename T, typename P1, typename P2, typename P3> static void Call(LuaState&, T f, P1 p1, P2 p2, P3 p3)
        { f(p1, p2, p3); }
        template<typename T, typename P1, typename P2, typename P3, typename P4> static void Call(LuaState&, T f, P1 p1, P2 p2, P3 p3, P4 p4)
        { f(p1, p2, p3, p4); }
        template<typename T, typename P1, typename P2, typename P3, typename P4, typename P5> static void Call(LuaState&, T f, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
        { f(p1, p2, p3, p4, p5); }
    };

    template<typename Signature>
    struct Functor : Callable
    {
        typedef typename Signature::Prototype FunctionType;
        FunctionType _f;

        Functor(FunctionType f) : _f(f) {}

        virtual void operator()(LuaState& api)
        {
            //std::cout << "FUNCTOR CALL (top=" << api.gettop() << ")\n";
            Caller<typename Signature::ReturnType, typename Signature::Parameters>::Call(api, _f);
        }
    };

    extern "C" {
        int _Tools_Lua_FunctorCall(lua_State* statePtr)
        {
            LuaState state(statePtr);
            Callable* s = (Callable*) state.topointer(state.upvalueindex(1));
            (*s)(state);
            return 1;
        }
    }

    template<typename Sig, typename Func>
    void Interpreter::_Bind(Func f)
    {
        Callable* functor = new Functor<Sig>(f);
        _bound.push_back(functor);
        api.pushlightuserdata(functor);
        api.pushcclosure(&_Tools_Lua_FunctorCall, 1);
    }

    /////////////////////////////////////////////////////////////////////////
    // Object Bind(Func, Args...)
    template<typename Func>
    Object Interpreter::Bind(Func func)
    {
        Interpreter::_Bind<Tools::Meta::Signature<Func>>(std::bind(func));
        StackRefPtr newRef(new StackRef(api));
        return Object(newRef);
    }

    template<typename Func, typename T1>
    Object Interpreter::Bind(Func func, T1 t1)
    {
        Interpreter::_Bind<Tools::Meta::Signature<Func>>(std::bind(func, t1));
        StackRefPtr newRef(new StackRef(api));
        return Object(newRef);
    }

    template<typename Func, typename T1, typename T2>
    Object Interpreter::Bind(Func func, T1 t1, T2 t2)
    {
        Interpreter::_Bind<Tools::Meta::Signature<Func>>(std::bind(func, t1, t2));
        StackRefPtr newRef(new StackRef(api));
        return Object(newRef);
    }

    template<typename Func, typename T1, typename T2, typename T3>
    Object Interpreter::Bind(Func func, T1 t1, T2 t2, T3 t3)
    {
        Interpreter::_Bind<Tools::Meta::Signature<Func>>(std::bind(func, t1, t2, t3));
        StackRefPtr newRef(new StackRef(api));
        return Object(newRef);
    }

    template<typename Func, typename T1, typename T2, typename T3, typename T4>
    Object Interpreter::Bind(Func func, T1 t1, T2 t2, T3 t3, T4 t4)
    {
        Interpreter::_Bind<Tools::Meta::Signature<Func>>(std::bind(func, t1, t2, t3, t4));
        StackRefPtr newRef(new StackRef(api));
        return Object(newRef);
    }

    /////////////////////////////////////////////////////////////////////////
    // Object Bind(char const*, Func, Args...)
    template<typename Func>
    Object Interpreter::Bind(char const* str, Func func)
    {
        Interpreter::_Bind<Tools::Meta::Signature<Func>>(std::bind(func));
        api.setglobal(str);
        return (*this)[str];
    }

    template<typename Func, typename T1>
    Object Interpreter::Bind(char const* str, Func func, T1 t1)
    {
        Interpreter::_Bind<Tools::Meta::Signature<Func>>(std::bind(func, t1));
        api.setglobal(str);
        return (*this)[str];
    }

    template<typename Func, typename T1, typename T2>
    Object Interpreter::Bind(char const* str, Func func, T1 t1, T2 t2)
    {
        Interpreter::_Bind<Tools::Meta::Signature<Func>>(std::bind(func, t1, t2));
        api.setglobal(str);
        return (*this)[str];
    }

    template<typename Func, typename T1, typename T2, typename T3>
    Object Interpreter::Bind(char const* str, Func func, T1 t1, T2 t2, T3 t3)
    {
        Interpreter::_Bind<Tools::Meta::Signature<Func>>(std::bind(func, t1, t2, t3));
        api.setglobal(str);
        return (*this)[str];
    }

    template<typename Func, typename T1, typename T2, typename T3, typename T4>
    Object Interpreter::Bind(char const* str, Func func, T1 t1, T2 t2, T3 t3, T4 t4)
    {
        Interpreter::_Bind<Tools::Meta::Signature<Func>>(std::bind(func, t1, t2, t3, t4));
        api.setglobal(str);
        return (*this)[str];
    }

}}

#endif
