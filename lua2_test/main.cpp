#include "tools/lua2/Interpreter.hpp"
#include "tools/lua2/Ref.hpp"

void f(Tools::Lua::Call& call)
{
}

int main(int, char**)
{
    Tools::Lua::Interpreter i;

    i.DoString("test = {} test[4] = 23.34");

    Tools::Lua::Ref bite = i.MakeFunction(std::bind(&f, std::placeholders::_1));

    Tools::Lua::Call call(i);
    bite(call);

    return 0;
}
