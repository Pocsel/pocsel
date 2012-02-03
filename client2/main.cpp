#include "client2/Client.hpp"

int main(int ac, char** av)
{
    Client::Client client(ac, av);
    return client.Run();
}
