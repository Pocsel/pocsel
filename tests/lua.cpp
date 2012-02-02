
#include<iostream>
#include <cassert>

#include "tools/lua/Interpreter.hpp"


int main()
{
    Tools::Lua::Interpreter i;
    i.Exec(
        "print(\"pif = \", pif)\na=12\nb=457.45\ns =\"une string !\"\n"
        "t = {k = \"Ceci est un test\", other=\"bite\"}\n"
        "v = {21, 5, 32}\n"
        "print(\"MEGATIEB\")\n"
    );
    Tools::Lua::Object v = i["v"];
    Tools::Lua::Object t = i["t"];
    Tools::Lua::Object a = i["a"];
    std::cout << "v type is " << v.GetType() << std::endl;
    std::cout << "t.k type is " << t["k"].GetType() << std::endl;

    std::cout << a.GetIndex() << ": a = " << static_cast<int>(a) << std::endl
              << i["b"].GetIndex() << ": b = " << i["b"].as<float>() << std::endl
              << "s = " << i["s"].as<std::string>() << std::endl
              << "typeof(t) = " << t.GetType() << std::endl
              << "t.k = " << t["k"].ToString() << std::endl
              << "v[1] = " << v[1].as<int>() << std::endl
              << "v[2] = " << v[2].as<int>() << std::endl
              << "v[3] = " << v[3].GetType() << std::endl;

    i.DumpStack();
    for (auto it = t.begin(), end = t.end(); it != end; ++it)
    {
        std::cout << " - " << it.key().as<std::string>() << ' ' << (*it).as<std::string>() << std::endl;
    }

    for (auto it = t.begin(), end = t.end(); it != end; ++it)
    {
        std::cout << " - " << it.key().as<std::string>() << ' ' << (*it).as<std::string>() << std::endl;
    }
    std::cout << "Vector\n";

    for (auto it = v.begin(), end = v.end(); it != end; ++it)
    {
        std::cout << " - " << it.key().as<int>() << ':' << (*it).as<int>() << std::endl;
    }

    auto o = i["notfound"], o2 = i["a"];

    if (o.IsNil())
        std::cout << "notfound is nil" << std::endl;
    o.CreateTable();
    if (o.IsTable())
        std::cout << "notfound is now a table" << std::endl;
    std::cout << "tests done.\n";
    return 0;
}
