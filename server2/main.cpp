#include <iostream>
#include <sstream>
#include <stdexcept>

#include "common/constants.hpp"

#include "server2/Server.hpp"

static Server::Server* server2;

#ifndef _WIN32
# include <signal.h>
void sigint(int)
{
    extern Server::Server* server2;
    if (server2 != 0)
        server2->Stop();
}
void sigpipe(int)
{
    std::cerr << "Broken pipe catched !" << std::endl;
}
#endif

int main(int ac, char *av[])
{
#ifndef WIN32
    (void) ::signal(SIGINT, sigint);
    (void) ::signal(SIGPIPE, sigpipe);
#endif

    server2 = new Server::Server(ac, av);
    int res;
    try
    {
        res = server2->Run();
    }
    catch (std::exception const& err)
    {
        std::cerr << "Unexpected exception caught: " << err.what() << std::endl;
        res = 1;
    }
    Tools::Delete(server2);
    return res;
}
