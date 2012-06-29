#ifndef __TOOLS_LUA_UTILS_UTILS_HPP__
#define __TOOLS_LUA_UTILS_UTILS_HPP__

namespace Tools { namespace Lua {
    class Interpreter;
}}

namespace Tools { namespace Lua { namespace Utils {

    void RegisterVector(Lua::Interpreter& interpreter);
    void RegisterMatrix(Lua::Interpreter& interpreter);

}}}

#endif
