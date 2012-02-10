#include "common/constants.hpp"

#include "server2/Server.hpp"

static Server::Server* server2;

#ifndef _WIN32
# include <signal.h>
void sigint(int)
{
    if (server2 != 0)
        server2->Stop();
}
#else
BOOL WINAPI ConsoleControlHandler(DWORD control)
{
    if (control == CTRL_C_EVENT || control == CTRL_BREAK_EVENT || control == CTRL_CLOSE_EVENT || control == CTRL_SHUTDOWN_EVENT)
    {
        if (server2 != 0)
            server2->Stop();
        return true;
    }
    return false;
}
#endif

int main(int ac, char *av[])
{
#ifndef WIN32
    (void) ::signal(SIGINT, sigint);
#else
    SetConsoleCtrlHandler(ConsoleControlHandler, true);
#endif

    server2 = new Server::Server(ac, av);
    int res;
    try
    {
        res = server2->Run();
    }
    catch (std::exception const& err)
    {
        Tools::error << "Unexpected exception caught: " << err.what() << "\n";
        res = 1;
    }
    Tools::Delete(server2);
    return res;
}
