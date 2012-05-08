#include <boost/cstdlib.hpp>

#include "installer-cli/ProgramInfo.hpp"
#include "tools/plugin-install/Install.hpp"

namespace {

    void Usage()
    {
        Tools::log << "Usage: " PROGRAM_NAME " PLUGIN_FILE WORLD_FILE [WORLD_IDENTIFIER WORLD_FULLNAME]" << std::endl;
        exit(boost::exit_failure);
    }

}

int main(int ac, char** av)
{
    bool success;
    switch (ac)
    {
        case 3:
            success = Tools::PluginInstall::Install(av[1], av[2]);
            break;
        case 5:
            success = Tools::PluginInstall::Install(av[1], av[2], av[3], av[4]);
            break;
        default:
            Usage();
            success = false;
    }
    return success ? boost::exit_success : boost::exit_failure;
}
