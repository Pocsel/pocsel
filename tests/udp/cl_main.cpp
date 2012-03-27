#include "precompiled.hpp"

#include "tools/Timer.hpp"
#include "cl_network.hpp"

int main(int ac, char** av)
{
    std::cout << "USAGE: ./toto IP <port=8173>\n";
    char const* host = "127.0.0.1";
    char const* port = "8173";

    if (ac >= 2)
        host = av[1];
    if (ac >= 3)
        port = av[2];

    std::cout << "HOST: " << host << ", PORT: " << port << "\n";

    cl::Network network;
    network.Connect(host, port);

    while (1)
        Tools::Timer::Sleep(2000);

    return 0;
}
