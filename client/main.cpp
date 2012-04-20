#include "client/Settings.hpp"
#include "client/Client.hpp"

#undef main

int main(int ac, char** av)
{
    int ret;
    {
        auto settings = Client::Settings(ac, av);
        Client::Client client(settings);
        ret = client.Run();
    }
    _CrtDumpMemoryLeaks();
    return ret;
}
