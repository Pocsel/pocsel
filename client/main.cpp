#include "client/Settings.hpp"
#include "client/Client.hpp"

#undef main

#ifdef _MSC_VER && DEBUG
static void DumpMemory()
{
    _CrtDumpMemoryLeaks();
}
#endif

int main(int ac, char** av)
{
#ifdef _MSC_VER && DEBUG
    atexit(&DumpMemory);
#endif

    auto settings = Client::Settings(ac, av);
    Client::Client client(settings);
    return client.Run();
}
