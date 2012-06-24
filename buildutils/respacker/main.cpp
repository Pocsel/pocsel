#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

static int PackResource(std::string in, std::string out)
{
    std::ifstream resource(in);
    std::ofstream output(out);

    if (!resource.is_open())
    {
        std::cout << "Can't open " << in << std::endl;
        return EXIT_FAILURE;
    }
    if (!output.is_open())
    {
        std::cout << "Can't open " << out << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Resource: \"" << in << "\" -> \"" << out  << "\"."<< std::endl;

    output << "static unsigned char const data[] = {" << std::endl;
    while (true)
    {
        unsigned int nb = (unsigned char)resource.get();
        if (!resource.eof())
            output << "0x" << std::hex << nb << ",";
        else
            break;
    }
    output << std::endl << "};" << std::endl;

    return EXIT_SUCCESS;
}

int main(int ac, char** av)
{
    if (ac < 3)
    {
        std::cout << "Usage: " << (ac > 0 ? av[0] : "respacker") << " <resource file> <output C file>" << std::endl;
        return EXIT_FAILURE;
    }

    return PackResource(av[1], av[2]);
}
