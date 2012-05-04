#include <boost/cstdlib.hpp>

#include "packager-cli/ProgramInfo.hpp"
#include "tools/plugin-create/Create.hpp"

namespace {

    void Usage()
    {
        Tools::log << "Usage: " PROGRAM_NAME " PLUGIN_ROOT_DIRECTORY DESTINATION_FILE" << std::endl;
        exit(boost::exit_failure);
    }

}

int main(int ac, char** av)
{
    if (ac != 3)
        Usage();
    return Tools::PluginCreate::Create(av[1], av[2]) ? boost::exit_success : boost::exit_failure;
}
