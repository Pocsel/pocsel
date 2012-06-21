#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>

int main(int ac, char** av)
{
    if (ac != 3)
    {
        std::cout << "Usage: " << (ac > 0 ? av[0] : "buildutils") << " <resource file> <output C file>" << std::endl;
        return EXIT_FAILURE;
    }
    std::ifstream resource(av[1]);
    std::ofstream output(av[2]);

    if (!resource.is_open())
    {
        std::cout << "Can't open " << av[1] << std::endl;
        return EXIT_FAILURE;
    }
    if (!output.is_open())
    {
        std::cout << "Can't open " << av[2] << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Resource: \"" << av[1] << "\" to header \"" << av[2]  << "\"."<< std::endl;

    output << "static unsigned char const data[] = {" << std::endl;
    while (!resource.eof())
    {
        unsigned int nb = (unsigned char)resource.get();
        output << "0x" << std::hex << nb << ",";
    }
    output << std::endl << "};" << std::endl;

    return EXIT_SUCCESS;
}