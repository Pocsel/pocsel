
#include <iostream>

#include "tools/lua/Interpreter.hpp"

static std::string code =
    "equations = {\n"
    "    perlin2 = {\n"
    "        name = \"perlin2\",\n"
    "        xn = 0.00130,\n"
    "        xa = -0.0112,\n"
    "        yn = 0.00139,\n"
    "        ya = 0.0761,\n"
    "        mul = 123.5545,\n"
    "        alpha = 1.7,\n"
    "        beta = 2.0,\n"
    "        n = 8\n"
    "    },\n"
    "    perlin3 = {\n"
    "        name = \"perlin3\",\n"
    "        xn = 0.063,\n"
    "        xa = -0.0937,\n"
    "        yn = 0.142,\n"
    "        ya = 0.0841,\n"
    "        zn = 0.063,\n"
    "        za = 0.0764,\n"
    "        mul = 1.0,\n"
    "        alpha = 2.0,\n"
    "        beta = 2.0,\n"
    "        n = 1\n"
    "    }\n"
    "}\n"
    "\n"
    "cube_confs = {\n"
    "    empty = {\n"
    "        priority = 0,\n"
    "        equation = \"perlin3\",\n"
    "        validator = {\n"
    "            name = \"superior\",\n"
    "            value = 0.19\n"
    "        }\n"
    "    },\n"
    "    grass = {\n"
    "        priority = 1,\n"
    "        equation = \"perlin2\",\n"
    "        validator = {\n"
    "            name = \"grass_validator\"\n"
    "        }\n"
    "    },\n"
    "    iron = {\n"
    "        priority = 2,\n"
    "        equation = \"perlin3\",\n"
    "        validator = {\n"
    "            name = \"superior\",\n"
    "            value = 0.15\n"
    "        }\n"
    "    },\n"
    "    rock = {\n"
    "        priority = 3,\n"
    "        equation = \"perlin2\",\n"
    "        validator = {\n"
    "            name = \"rock_validator\"\n"
    "        }\n"
    "    },\n"
    "    dirt = {\n"
    "        priority = 5,\n"
    "        equation = \"perlin2\",\n"
    "        validator = {\n"
    "            name = \"y_inferior\"\n"
    "        }\n"
    "    }\n"
    "}\n"
    ;

int main()
{
    Tools::Lua::Interpreter i(code);

    auto equations = i["equations"];
    std::cout
        << "Equations: " << equations << " len = " << i.api.objlen(equations.GetIndex()) << std::endl
    ;
    for (auto it = i["equations"].begin(), end = equations.end(); it != end; ++it)
    {
        auto k = it.key(),
             v = *it;
        std::cout << "\t- " << k << ": " << v << std::endl;
        std::cout << "\t\tname = " << v["name"] << std::endl;
        std::cout << "\t\txn = " << v["xn"] << std::endl;
        std::cout << "\t\txa = " << v["xa"] << std::endl;
        std::cout << "\t\tyn = " << v["yn"] << std::endl;
        std::cout << "\t\tya = " << v["ya"] << std::endl;
        std::cout << "\t\tmul = " << v["mul"] << std::endl;
        std::cout << "\t\talpha = " << v["alpha"] << std::endl;
        std::cout << "\t\tbeta = " << v["beta"] << std::endl;
        std::cout << "\t\tn = " << v["n"] << std::endl;
    }



    auto cube_confs = i["cube_confs"];
    std::cout
        << "cube_confs: " << cube_confs << " len = " << i.api.objlen(cube_confs.GetIndex()) << std::endl
    ;
    auto it = cube_confs.begin(), end =  cube_confs.end();
    for (; it != end; ++it)
    {
        auto k = it.key(),
             v = *it;
        std::cout << "\t- " << k << ": " << v << std::endl;
        std::cout << "\t\tequation = " << v["equation"] << std::endl;
        std::cout << "\t\tpriority = " << v["priority"] << std::endl;
        std::cout << "\t\tvalidator.name = " << v["validator"]["name"] << std::endl;
        std::cout << "\t\tvalidator.value = " << v["validator"]["value"] << std::endl;
    }

    std::cout << "tests done.\n";
    return 0;
}
