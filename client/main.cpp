#include "client/Settings.hpp"
#include "client/Client.hpp"

#undef main

#if defined _MSC_VER && defined DEBUG
static void DumpMemory()
{
    _CrtDumpMemoryLeaks();
}
#endif

int main(int ac, char** av)
{
#if defined _MSC_VER && defined DEBUG
    atexit(&DumpMemory);
#endif

    auto settings = Client::Settings(ac, av);
    Client::Client client(settings);
    return client.Run();
}
