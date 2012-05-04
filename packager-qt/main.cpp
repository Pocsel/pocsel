#include "packager-qt/Packager.hpp"

int main(int ac, char** av)
{
    PackagerQt::Packager packager(ac, av);
    return packager.Run();
}
