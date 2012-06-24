#include "tools/precompiled.hpp"

#include "tools/lua/Interpreter.hpp"
#include "tools/lua/utils/Utils.hpp"

namespace Tools { namespace Lua { namespace Utils {

#define data luaMatrixCode
#include "tools/lua/utils/Matrix.lua.hpp"
#undef data

    void RegisterMatrix(Lua::Interpreter& interpreter)
    {
        RegisterVector(interpreter);
        if (!interpreter.Globals().GetTable("Utils")["Matrix"].Exists())
            interpreter.DoString(std::string((char const*)luaMatrixCode, sizeof(luaMatrixCode)));
    }

}}}
