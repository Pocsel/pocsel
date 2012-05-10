#include "sdk/packager-qt/Packager.hpp"

int main(int ac, char** av)
{
    Sdk::PackagerQt::Packager packager(ac, av);
    return packager.Run();
}
