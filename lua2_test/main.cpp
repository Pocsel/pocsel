#include "tools/lua2/Interpreter.hpp"
#include "tools/lua2/Ref.hpp"
#include "tools/lua2/Iterator.hpp"

void f(Tools::Lua::CallHelper& call)
{
    auto const& args = call.GetArgList();
    auto it = args.begin();
    auto itEnd = args.end();
    for (; it != itEnd; ++it)
        Tools::log << "arg: " << it->ToNumber() << Tools::endl;
    call.PopArg();
    call.PopArg();
    call.PopArg();
    call.PopArg();
    call.PopArg();
    Tools::log << "f()" << Tools::endl;
    call.PushRet(call.GetInterpreter().MakeNumber(123.2));
    call.PushRet(call.GetInterpreter().MakeNumber(-5.4));
}

int main(int, char**)
{
    Tools::Lua::Interpreter i;

    i.DoString("test = {} test[4] = 23.34");

    Tools::Lua::Ref bite = i.MakeFunction(std::bind(&f, std::placeholders::_1));

    try
    {
        Tools::log << bite(12, 14.4f).ToNumber() << " jksdfgjk " << sizeof(Tools::Lua::Ref) << Tools::endl;
    }
    catch (std::exception& e)
    {
        Tools::log << "from c++: " << e.what() << Tools::endl;
    }

    i.Globals().Set(std::string("bite"), bite);

    try
    {
        i.DoString("bite(13.3, 13.4)");
    }
    catch (std::exception& e)
    {
        Tools::log << "from lua: " << e.what() << Tools::endl;
    }

    i.DoString("test = \"sjkldglkhsdfg\" g = 323.4 jdsk = {} iwer = {} jkd = function() end");

    Tools::Lua::Iterator it = i.Globals().Begin();
    Tools::Lua::Iterator itEnd = i.Globals().End();
    for (; it != itEnd; ++it)
    {
        Tools::log << it.GetValue().TypeName() << ": " << it.GetValue().ToString() << " (key " << it.GetKey().TypeName() << " " << it.GetKey().ToString() << ")" << Tools::endl;
    }

    return 0;
}
