#include "client2/Settings.hpp"
#include "client2/Client.hpp"

int main(int ac, char** av)
{
    auto settings = Client::Settings(ac, av);
    Client::Client client(settings);
    return client.Run();
}
