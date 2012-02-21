#include "tools/lua2/Interpreter.hpp"
#include "tools/lua2/Var.hpp"

int main(int, char**)
{
    Tools::Lua::Interpreter i;
    Tools::Lua::Var v(i);

    Tools::log << v.IsNil() << Tools::endl;
    Tools::log << v.GetType() << Tools::endl;

    i.DumpStack();

    v.FromGlobalTable("test");
    Tools::log << v.GetType() << Tools::endl;

    i.DumpStack();

    try
    {
        i.DoString("test = 12");
    }
    catch (std::exception& e)
    {
        Tools::log << e.what() << Tools::endl;
    }

    Tools::log << v.GetType() << Tools::endl;

    v.FromGlobalTable("test");
    Tools::log << v.ToDouble() << Tools::endl;

    i.DumpStack();

    return 0;
}
