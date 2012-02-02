
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "tools/MessageQueueManager.hpp"

#include "common/constants.hpp"

#include "server/Server.hpp"

static Server::Server* server;

#ifndef _WIN32
# include <signal.h>
void sigint(int)
{
    extern Server::Server* server;
    if (server != 0)
        server->Stop();
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

    server = new Server::Server(ac, av);
    Tools::MessageQueueManager::Start(10);
    int res;
    try
    {
        res = server->Run();
    }
    catch (std::exception const& err)
    {
        std::cerr << "Unexpected exception caught: " << err.what() << std::endl;
        res = 1;
    }
    Tools::MessageQueueManager::Stop();
    delete server;
    server = 0;
    return res;
}

