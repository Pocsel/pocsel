
#include <iostream>

#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Bind.hpp"

struct A
{
    int getint() {return 42;}
    void printme(std::string const& s){
        std::cout << s;
    }
    void print2x(std::string const& s1, std::string const& s2){
        std::cout << s1 << " + " << s2;
    }
};

int main()
{
    Tools::Lua::Interpreter i;
    A a;
    i.Bind("getint", &A::getint, &a);
    i.Bind("printme", &A::printme, &a, std::placeholders::_1);
    i.Bind("print2x", &A::print2x, &a, std::placeholders::_1, std::placeholders::_2);
    i.Exec("print(\"BITE\", getint())");
    i.Exec("printme(\"Yeah printed \\n\")");
    i.Exec("print2x(\"second try\", \"two times !\\n\")");
    i.Exec("function f()\n"
           "    print(\"0: lua func called from C++\")\n"
           "end\n");
    i["f"]();
    i.Exec("function f2(p)\n"
           "    print(\"1: lua func called from C++\", p)\n"
           "end\n");
    i["f2"](121);
    i["f2"]("Salut");
    i["f2"](3.14159265f);
    i.Exec("function f3(p, k)\n"
           "    print(\"2: lua func called from C++\", p, k)\n"
           "end\n");
    i["f3"](42, 32);
    i["f3"]("Hello,", " World!");
    i["f3"]("Status:", "tests done.");
    return 0;
}

