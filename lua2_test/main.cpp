#include "tools/lua2/Interpreter.hpp"
#include "tools/lua2/Ref.hpp"

int main(int, char**)
{
    Tools::Lua::Interpreter i;

    i.DoString("test = { test2 = 23 }");

    Tools::Lua::Ref r1 = i["test"];
    Tools::Lua::Ref r2 = i["test"]["test2"];
    Tools::Lua::Ref r3 = r2;
    Tools::Lua::Ref r4(r1);

    Tools::log << "r1 " << r1.ToNumber() << Tools::endl;
    Tools::log << "r2 " << r2.ToNumber() << Tools::endl;
    Tools::log << "r3 " << r3.ToNumber() << Tools::endl;
    Tools::log << "r4 " << r4.ToNumber() << Tools::endl;

    return 0;
}
