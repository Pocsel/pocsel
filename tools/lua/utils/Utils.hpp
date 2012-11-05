#ifndef __TOOLS_LUA_UTILS_UTILS_HPP__
#define __TOOLS_LUA_UTILS_UTILS_HPP__

namespace Luasel {
    class Interpreter;
}

namespace Tools { namespace Lua { namespace Utils {

    void RegisterVector(Luasel::Interpreter& interpreter);
    void RegisterMatrix(Luasel::Interpreter& interpreter);

}}}

#endif
