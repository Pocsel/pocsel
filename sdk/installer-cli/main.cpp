#include <boost/cstdlib.hpp>

#include "sdk/installer-cli/ProgramInfo.hpp"
#include "tools/plugin-install/Install.hpp"
#include "common/constants.hpp"

namespace {

    void Usage()
    {
        Tools::log << "Usage: " PROGRAM_NAME " PLUGIN_FILE WORLD_FILE [WORLD_IDENTIFIER WORLD_FULLNAME [--recreate]]" << std::endl;
        exit(boost::exit_failure);
    }

    bool Install(boost::filesystem::path const& pluginFile, boost::filesystem::path const& worldFile, std::string const& identifier = "", std::string const& fullname = "")
    {
        if (boost::filesystem::is_directory(pluginFile))
        {
            Tools::log << "Installing all plugins found in directory " << pluginFile << "." << std::endl;
            bool atLeastOne = false;
            boost::filesystem::directory_iterator it(pluginFile);
            boost::filesystem::directory_iterator itEnd;
            for (; it != itEnd; ++it)
                if (boost::filesystem::is_regular_file(*it) && it->path().extension() == ("." + std::string(Common::PluginFileExt)))
                {
                    atLeastOne = true;
                    if (!Tools::PluginInstall::Install(it->path(), worldFile, identifier, fullname))
                        return false;
                }
            if (!atLeastOne)
            {
                Tools::log << "No plugins to install." << std::endl;
                return false;
            }
            return true;
        }
        else
            return Tools::PluginInstall::Install(pluginFile, worldFile, identifier, fullname);
    }

}

int main(int ac, char** av)
{
    bool success;
    switch (ac)
    {
        case 3:
            success = Install(av[1], av[2]);
            break;
        case 5:
            success = Install(av[1], av[2], av[3], av[4]);
            break;
        case 6:
            if (std::string(av[5]) == "--recreate")
            {
                if (boost::filesystem::is_regular_file(av[2]))
                {
                    boost::filesystem::remove(av[2]);
                    Tools::log << "Removed old world \"" << av[2] << "\"." << std::endl;
                }
                success = Install(av[1], av[2], av[3], av[4]);
                break;
            }
        default:
            Usage();
            success = false;
    }
    return success ? boost::exit_success : boost::exit_failure;
}
