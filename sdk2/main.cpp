#include "sdk2/Sdk.hpp"

int main(int ac, char** av)
{
    Sdk::Sdk sdk(ac, av);
    return sdk.Run();
}
