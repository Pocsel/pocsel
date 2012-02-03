
#include <iostream>
#include <cstddef>
#include <cassert>

extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

#include "tools/meta/StaticIf.hpp"
#include "tools/meta/Equals.hpp"
#include "tools/meta/IsConvertible.hpp"

#include "LuaState.hpp"
#include "Exception.hpp"
#include "Object.hpp"
#include "StackRefPtr.hpp"
#include "StackRef.hpp"

#ifdef _MSC_VER
# pragma warning(disable: 4800)
#endif

// Jérémy: Je ne sais pas pourquoi mais ça ne compile pas à cause des concaténations au niveau des throw sans ça... :/
std::string operator +(std::string const& s1, std::string const& s2)
{
    return std::string().append(s1).append(s2);
}

using namespace Tools::Lua;

static const luaL_reg lualibs[] =
{
    { "base",       &luaopen_base },
    { "table",      &luaopen_table },
    { NULL,         NULL }
};

LuaState::LuaState() :
    _statePtr(lua_open()),
    _state(*_statePtr)
{
    if (!_statePtr)
        throw std::runtime_error("Out of memory");
    for (const luaL_reg* lib = lualibs; lib->func != NULL; lib++)
    {
        lib->func(_statePtr);
        lua_settop(_statePtr, 0);
    }
}

LuaState::LuaState(lua_State* statePtr) :
    _statePtr(0), // prevent destrutor close
    _state(*statePtr)
{}

LuaState::~LuaState()
{
    if (_statePtr)
        lua_close(_statePtr);
}

char const* LuaState::typestring(LuaState::Idx idx)
{
    return lua_typename(&_state, lua_type(&_state, idx));
}

void LuaState::loadstring(const char* str)
{
    if (luaL_loadstring(&_state, str) != 0)
        throw RuntimeError(
                std::string("Cannot load string: ") + std::string(lua_tostring(&_state, -1))
                );
}

void LuaState::dostring(const char* str)
{
    if (luaL_dostring(&_state, str) != 0)
        throw RuntimeError(
                std::string("Cannot exec string: ") + std::string(lua_tostring(&_state, -1))
                );
}

void LuaState::dofile(const char* path)
{
    if (luaL_dofile(&_state, path) != 0)
        throw RuntimeError(
                std::string("Cannot exec file: ") + lua_tostring(&_state, -1)
                );
}

void LuaState::checkstack(int extra)
{
    if (!lua_checkstack(&_state, extra))
        throw RuntimeError("Out of memory");
}

LuaState::Idx LuaState::ref()
{
    Idx r = luaL_ref(&_state, LUA_REGISTRYINDEX);
#ifdef DEBUG
    if (r != LUA_REFNIL)
    {
        assert(this->_references.find(r) == this->_references.end());
        this->_references.insert(r);
    }
#endif
    return r;
}

void LuaState::getref(LuaState::Idx idx)
{
#ifdef DEBUG
    if (idx != LUA_REFNIL)
        assert(this->_references.find(idx) != this->_references.end());
#endif
    return rawgeti(LUA_REGISTRYINDEX, idx);
}

void LuaState::unref(LuaState::Idx idx)
{
#ifdef DEBUG
    if (idx != LUA_REFNIL)
    {
        assert(this->_references.find(idx) != this->_references.end());
        this->_references.erase(idx);
    }
#endif
    luaL_unref(&_state, LUA_REGISTRYINDEX, idx);
}

LuaState::Idx LuaState::upvalueindex(LuaState::Idx idx)
{
    return lua_upvalueindex(idx);
}

#define APICALL(RetType, method) \
RetType LuaState::method() { return lua_##method(&_state); }

#define APICALL1(RetType, method, ParamType1) \
RetType LuaState::method(ParamType1 p1) { return lua_##method(&_state, p1); }

#define APICALL2(RetType, method, P1, P2) \
RetType LuaState::method(P1 p1, P2 p2) { return lua_##method(&_state, p1, p2); }

#define APICALL3(RetType, method, P1, P2, P3) \
RetType LuaState::method(P1 p1, P2 p2, P3 p3) { return lua_##method(&_state, p1, p2, p3); }

typedef LuaState::Idx Idx;
typedef LuaState::Num Num;
typedef LuaState::Int Int;
typedef LuaState::Function Function;


APICALL1(int, type, Idx)

APICALL(Idx, gettop)
APICALL1(void, pop, Idx)
APICALL1(void, replace, Idx)
APICALL1(void, getglobal, char const*)
APICALL1(void, setglobal, char const*)

APICALL1(bool, isnone, Idx)
APICALL1(bool, isnil, Idx)
APICALL1(bool, isnoneornil, Idx)
APICALL1(bool, istable, Idx)
APICALL1(bool, isnumber, Idx)
APICALL1(bool, isboolean, Idx)
APICALL1(bool, isstring, Idx)
APICALL1(bool, isfunction, Idx)

APICALL(void, pushnil)
APICALL1(void, pushvalue, Idx)
APICALL1(void, pushstring, char const*)
APICALL1(void, pushboolean, bool)
APICALL1(void, pushnumber, Num)
APICALL1(void, pushinteger, Int)
APICALL1(void, pushlightuserdata, void*)
APICALL2(void, pushcclosure, Function, int)

APICALL1(char const*, tostring, Idx)
APICALL1(bool, toboolean, Idx)
APICALL1(Num, tonumber, Idx)
APICALL1(Int, tointeger, Idx)
APICALL1(void const*, topointer, Idx)

APICALL(void, newtable)
APICALL1(void, gettable, Idx)
APICALL1(void, settable, Idx)
APICALL2(void, rawgeti, Idx, Idx)
APICALL2(void, rawseti, Idx, Idx)
APICALL1(int, next, Idx)

APICALL3(int, pcall, int, int, Idx)

size_t LuaState::objlen(Idx table)
{
    if (istable(table))
    {
        size_t sz = 0;
        pushnil();
        while (next(table))
        {
            pop(1);
            sz++;
        }
        assert(istable(table));
        return sz;
    }
    else
    {
        return lua_objlen(&_state, table);
    }
}

namespace {

    template<typename T> struct NumberCaster
    {
        static inline T to(LuaState& api, Idx idx)
        {
            return static_cast<T>(api.tonumber(idx));
        }

        static inline void push(LuaState& api, T val)
        {
            return api.pushnumber(val);
        }
    };

    template<typename T> struct BoolCaster
    {
        static inline T to(LuaState& api, Idx idx)
        {
            return static_cast<T>(api.toboolean(idx));
        }
        static inline void push(LuaState& api, T val)
        {
            return api.pushboolean(val);
        }
    };

    template<typename T> struct StringCaster
    {
        static inline T to(LuaState& api, Idx idx)
        {
            return static_cast<T>(api.tostring(idx));
        }

        static inline void push(LuaState& api, T val)
        {
            return api.pushstring(val);
        }
    };

    template<> struct StringCaster<std::string>
    {
        static inline std::string to(LuaState& api, Idx idx)
        {
            return api.tostring(idx);
        }

        static inline void push(LuaState& api, std::string val)
        {
            return api.pushstring(val.c_str());
        }
    };

    template<> struct StringCaster<std::string const&>
    {
        static inline std::string to(LuaState& api, Idx idx)
        {
            return api.tostring(idx);
        }

        static inline void push(LuaState& api, std::string const& val)
        {
            return api.pushstring(val.c_str());
        }
    };

    struct ObjectCaster
    {
        static inline Object to(LuaState& api, Idx idx)
        {
            api.pushvalue(idx);
            StackRefPtr newRef(new StackRef(api));
            return Object(newRef, 0);
        }

        static inline void push(LuaState& api, Object val)
        {
            api.getref(val.GetIndex());
        }
    };

    template<typename T> struct UnknownCaster
    {
        static inline T to(LuaState& api, Idx idx)
        {
            static_assert(sizeof(T) == 0, "Cannot cast to this type");
        }
        static inline void push(LuaState& api, T val)
        {
            static_assert(sizeof(T) == 0, "Cannot cast to this type");
        }
    };

    template<typename T> struct Caster :
        Tools::Meta::StaticIf<
            Tools::Meta::Equals<Object, T>::result,
            ObjectCaster,
            typename Tools::Meta::StaticIf<
                Tools::Meta::Equals<bool, T>::result,
                BoolCaster<T>,
                typename Tools::Meta::StaticIf<
                    Tools::Meta::IsConvertible<double, T>::result,
                    NumberCaster<T>,
                    typename Tools::Meta::StaticIf<
                        Tools::Meta::IsConvertible<
                            typename Tools::Meta::RemoveReference<T>::Result,
                            std::string
                        >::result,
                        StringCaster<T>,
                        UnknownCaster<T>
                    >::Result
                >::Result
            >::Result
        >::Result
    {};

}

template<typename T> void LuaState::push(T val)
{
    Caster<T>::push(*this, val);
}

template<typename T>
    typename Tools::Meta::RemoveQualifiers<
        typename Tools::Meta::RemoveReference<T>::Result
    >::Result LuaState::to(Idx idx)
{
    return Caster<T>::to(*this, idx);
}

#define FORCE_INSTANCE(Type) \
    template Tools::Meta::RemoveQualifiers< \
        typename Tools::Meta::RemoveReference<Type>::Result \
    >::Result LuaState::to<Type>(Idx); \
    template void LuaState::push<Type>(Type)

FORCE_INSTANCE(char);
FORCE_INSTANCE(int);
FORCE_INSTANCE(long);
FORCE_INSTANCE(unsigned char);
FORCE_INSTANCE(unsigned int);
FORCE_INSTANCE(unsigned long);
FORCE_INSTANCE(float);
FORCE_INSTANCE(double);
FORCE_INSTANCE(bool);
FORCE_INSTANCE(std::string);
FORCE_INSTANCE(std::string const&);
FORCE_INSTANCE(char const*);
FORCE_INSTANCE(Object);
