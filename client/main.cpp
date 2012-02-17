#include "client/Settings.hpp"
#include "client/Client.hpp"

int main(int ac, char** av)
{
    auto settings = Client::Settings(ac, av);
    Client::Client client(settings);
    return client.Run();
}
