#include "common/constants.hpp"

#include "server/Server.hpp"
#include "server/Settings.hpp"

static Server::Server* server = 0;

#ifndef _WIN32
# include <signal.h>
void sigint(int)
{
    if (server != 0)
        server->Stop();
}
#else
BOOL WINAPI ConsoleControlHandler(DWORD control)
{
    if (control == CTRL_C_EVENT || control == CTRL_BREAK_EVENT || control == CTRL_CLOSE_EVENT || control == CTRL_SHUTDOWN_EVENT)
    {
        if (server != 0)
            server->Stop();
        return true;
    }
    return false;
}

# if defined _MSC_VER && defined DEBUG
static void DumpMemory()
{
    _CrtDumpMemoryLeaks();
}
# endif

#endif

int main(int ac, char **av)
{
#ifndef WIN32
    (void) ::signal(SIGINT, sigint);
#else
    SetConsoleCtrlHandler(ConsoleControlHandler, true);
# if defined _MSC_VER && defined DEBUG
    atexit(&DumpMemory);
# endif
#endif

    Server::Settings settings(ac, av);
    server = new Server::Server(settings);
    int res;
    try
    {
        res = server->Run();
    }
    catch (std::exception const& err)
    {
        Tools::error << "Unexpected exception caught: " << err.what() << "\n";
        res = 1;
    }
    Tools::Delete(server);
    return res;
}
