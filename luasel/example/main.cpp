#include <iostream>
#include <luasel/Luasel.hpp>

int main(int, char**)
{
    Luasel::Interpreter i;
    i.RegisterLib(Luasel::Interpreter::Base);
    //i.RegisterLib(Luasel::Interpreter::Math);
    i.DoString("print('It is working :)')");
    return 0;
}
