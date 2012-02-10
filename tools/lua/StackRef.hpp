#ifndef __TOOLS_LUA_STACKREF_HPP__
#define __TOOLS_LUA_STACKREF_HPP__

#include "tools/lua/Object.hpp"
#include "tools/lua/StackRefPtr.hpp"

namespace Tools { namespace Lua {

    class LuaState;

    struct StackRef
    {
    public:
        LuaState& luaState;
        int idx;

    public:
        StackRef(LuaState& luaState) :
            luaState(luaState),
            idx(luaState.ref())
        {
            //std::cout << "Ref " << idx << "\n";
        }
        StackRef(StackRef&& other) :
            luaState(other.luaState),
            idx(other.idx)
        {}
        ~StackRef()
        {
            //std::cout << "Unref " << idx << std::endl;
            luaState.unref(idx);
        }
    };

}}

#endif
