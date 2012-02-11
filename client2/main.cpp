#include "client2/Options.hpp"
#include "client2/Client.hpp"

int main(int ac, char** av)
{
    Client::Options(ac, av);
    Client::Client client(ac, av);
    return client.Run();
}
