#ifndef __TOOLS_LUA_LUASTATE_HPP__
#define __TOOLS_LUA_LUASTATE_HPP__

#ifdef DEBUG
# include <set>
#endif

#include <cstddef>

#include <boost/noncopyable.hpp>

#include "tools/meta/RemoveQualifiers.hpp"
#include "tools/meta/RemoveReference.hpp"

extern "C" {
    struct lua_State;
}

namespace Tools { namespace Lua {

    class LuaState : private boost::noncopyable
    {
    public:
        typedef int Idx;
        typedef double Num;
        typedef ptrdiff_t Int;
        typedef int (*Function)(lua_State*);

    private:
        lua_State* _statePtr;
        lua_State& _state;
# ifdef DEBUG
        std::set<Idx> _references;
# endif

    public:
        LuaState();
        LuaState(lua_State* statePtr);
        ~LuaState();

        void loadstring(const char* str);
        void dostring(const char* str);
        void dofile(const char* path);
        void checkstack(int extra);
        Idx ref();
        void getref(Idx idx);
        void unref(Idx idx);
        Idx upvalueindex(Idx idx);

        char const* typestring(Idx idx);
        int type(Idx);

        Idx gettop();
        void pop(Idx idx);
        void replace(Idx idx);
        void getglobal(char const*);
        void setglobal(char const*);

        bool isnone(Idx);
        bool isnil(Idx);
        bool isnoneornil(Idx);
        bool istable(Idx);
        bool isnumber(Idx);
        bool isboolean(Idx);
        bool isstring(Idx);
        bool isfunction(Idx);

        void pushnil();
        void pushvalue(Idx idx);
        void pushstring(char const* val);
        void pushstring(std::string const& val) { pushstring(val.c_str()); }
        void pushboolean(bool val);
        void pushnumber(Num val);
        void pushinteger(Int val);
        void pushlightuserdata(void* data);
        void pushcclosure(Function f, int upvalues);
        template<typename T> void push(T val);

        char const* tostring(Idx idx);
        bool toboolean(Idx idx);
        Num tonumber(Idx idx);
        Int tointeger(Idx idx);
        void const* topointer(Idx idx);
        template<typename T>
            typename Tools::Meta::RemoveQualifiers<
                typename Tools::Meta::RemoveReference<T>::Result
            >::Result to(Idx idx);

        void newtable();
        void gettable(Idx idx);
        void settable(Idx idx);
        void rawgeti(Idx table, Idx idx);
        void rawseti(Idx table, Idx idx);
        int next(Idx table);
        size_t objlen(Idx table);

        int pcall(int nparams, int nresult, Idx error_hdlr);
    };

}}
#endif
