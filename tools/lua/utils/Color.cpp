#include "tools/precompiled.hpp"

#include "tools/lua/Interpreter.hpp"
#include "tools/lua/MetaTable.hpp"
#include "tools/lua/utils/Utils.hpp"
#include "tools/Color.hpp"

namespace {

    void _CreateColor(Tools::Lua::CallHelper& helper)
    {
        Tools::Color4f color(0, 0, 0, 1.0f);
        switch (helper.GetNbArgs())
        {
        default: throw std::invalid_argument("Can't create this color.");
        case 0: break;
        case 1:
            {
                auto arg = helper.PopArg();
                if (arg.IsString())
                    color = Tools::Color4f(arg.ToString());
                else
                {
                    int c = arg.ToInteger();
                    color.r = ((c >> 16) & 0xFF) / 255.0f;
                    color.g = ((c >> 8) & 0xFF) / 255.0f;
                    color.b = (c & 0xFF) / 255.0f;
                    color.a = ((c >> 24) & 0xFF) / 255.0f;
                }
            }
            break;
        case 4:
            color.a = (float)helper.PopArg().CheckNumber();
        case 3:
            color.r = (float)helper.PopArg().CheckNumber();
            color.g = (float)helper.PopArg().CheckNumber();
            color.b = (float)helper.PopArg().CheckNumber();
            break;
        }
        helper.PushRet(helper.GetInterpreter().Make(color));
    }
}

namespace Tools { namespace Lua { namespace Utils {

    void RegisterColor(Lua::Interpreter& interpreter)
    {
        Tools::Lua::MetaTable colorsMt(interpreter, Tools::Color4f());
        auto colorsNs = interpreter.Globals().GetTable("Utils").GetTable("Color");
        colorsNs.Set("Create", interpreter.MakeFunction(_CreateColor));
    }

}}}
