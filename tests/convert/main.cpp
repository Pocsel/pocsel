#include "tools/models/Convert.hpp"

int main(int ac, char** av)
{
    if (ac < 3)
    {
        std::cout << "usage: ./convert <outfile> <infile>";
        return 0;
    }

    Tools::Models::Convert convert;

    convert.Work(av[1], av[2]);

    return 0;
}
