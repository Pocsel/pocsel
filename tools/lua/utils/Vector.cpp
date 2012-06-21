#include "tools/precompiled.hpp"

#include "tools/lua/Interpreter.hpp"
#include "tools/lua/utils/Utils.hpp"

namespace Tools { namespace Lua { namespace Utils {

#define data luaVectorCode
#include "tools/lua/utils/Vector.lua.hpp"
#undef data

    void RegisterVec3(Lua::Interpreter& interpreter)
    {
        interpreter.DoString((char const*)luaVectorCode);
    }

    struct Test
    {
        Test() { RegisterVec3(Interpreter()); }
    } test;

}}}
