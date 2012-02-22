#include "tools/lua2/Interpreter.hpp"
#include "tools/lua2/Ref.hpp"

void f(Tools::Lua::Stack& stack)
{
}

int main(int, char**)
{
    Tools::Lua::Interpreter i;

    i.DoString("test = {} test[4] = 23.34");

    Tools::Lua::Ref bite = i.MakeFunction(std::bind(&f, std::placeholders::_1));

    Tools::Lua::Stack s(i);
    bite(s);

    return 0;
}
