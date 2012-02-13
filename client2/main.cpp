#include "client2/Settings.hpp"
#include "client2/Client.hpp"

int main(int ac, char** av)
{
    Client::Client client(Client::Settings(ac, av));
    return client.Run();
}
