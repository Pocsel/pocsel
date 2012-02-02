
#include <iostream>
#include <cassert>

#include "tools/lua/Interpreter.hpp"

int main()
{
    Tools::Lua::Interpreter i;

    auto a = i["a"];
    a.CreateTable();
    a["pif"] = 23;
    i["paf"].CreateTable()["pouf"] = 321;
    std::cout
        << "a.pif = " << a["pif"] << std::endl
        << "paf.pouf = " << i["paf"]["pouf"] << std::endl
        ;
    i.DumpStack();
    i.Exec("a.okydf = 132\nprint(a.okydf)\nprint(a.pif)\nprint(paf.pouf)\nprint(\"tests done.\")\n");
    return 0;
}
