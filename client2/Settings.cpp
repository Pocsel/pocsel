#include <boost/program_options.hpp>
#include <boost/cstdlib.hpp>

#include "client2/Settings.hpp"
#include "ProgramInfo.hpp"
#include "common/ConfDir.hpp"

namespace Client {

    Settings::Settings(int ac, char** av) :
        fps(60),
        chunkCacheDistance(5),
        nickname("Player")
    {
        std::string defaultConfDir = Common::ConfDir::Client().string();
        std::string defaultSettingsFile = defaultConfDir + "/settings.lua";
        std::string defaultBindingsFile = defaultConfDir + "/bindings.lua";
        std::string defaultCacheDir = defaultConfDir + "/cache";

        boost::program_options::options_description options("Options");
        options.add_options()
            ("host", boost::program_options::value<std::string>()->default_value("localhost"),
             "Address of a " PROJECT_NAME " server")
            ("port", boost::program_options::value<std::string>()->default_value("8173"),
             "Port of the " PROJECT_NAME " server")
            ("conf,c", boost::program_options::value<std::string>()->default_value(defaultConfDir),
             "Path to a client configuration directory")
            ("settings,s", boost::program_options::value<std::string>()->default_value(defaultSettingsFile),
             "Path to a settings file")
            ("bindings,b", boost::program_options::value<std::string>()->default_value(defaultBindingsFile),
             "Path to a bindings file")
            ("cache", boost::program_options::value<std::string>()->default_value(defaultCacheDir),
             "Path to a cache directory")
            ("version,v",
             "Show version and exit")
            ("help,h",
             "Show this help message and exit");

        boost::program_options::positional_options_description positional;
        positional.add("host", 1).add("port", 2);

        boost::program_options::variables_map vm;
        try
        {
            boost::program_options::store(boost::program_options::command_line_parser(ac, av).options(options).positional(positional).run(), vm);
            boost::program_options::notify(vm);
        }
        catch (std::exception& e)
        {
            Tools::error << "Invalid arguments (use -h for help): " << e.what() << Tools::endl;
            exit(boost::exit_failure);
        }

        if (vm.count("help"))
        {
            Tools::log << "Usage: " << (ac > 0 && av[0] ? av[0] : PROGRAM_NAME) << " [host [port]] [-c CONF] [-s SETTINGS] [-b BINDINGS] [--cache CACHE]\n\n";
            Tools::log << options;
            exit(boost::exit_success);
        }

        if (vm.count("version"))
        {
            Tools::log << PROJECT_NAME << " - " << PROGRAM_NAME << " - Version " << GIT_VERSION << Tools::endl;
            exit(boost::exit_success);
        }

        this->host = vm["host"].as<std::string>();
        this->port = vm["port"].as<std::string>();

        this->confDir = vm["conf"].as<std::string>();
        if (vm["conf"].as<std::string>() != defaultConfDir)
            Tools::log << "Configuration directory: " << this->confDir.string() << Tools::endl;

        if (vm["settings"].as<std::string>() == defaultSettingsFile)
            this->settingsFile = this->confDir / "settings.lua";
        else
        {
            this->settingsFile = vm["settings"].as<std::string>();
            Tools::log << "Settings file: " << this->settingsFile.string() << Tools::endl;
        }

        if (vm["bindings"].as<std::string>() == defaultBindingsFile)
            this->bindingsFile = this->confDir / "bindings.lua";
        else
        {
            this->bindingsFile = vm["bindings"].as<std::string>();
            Tools::log << "Bindings file: " << this->bindingsFile.string() << Tools::endl;
        }

        if (vm["cache"].as<std::string>() == defaultCacheDir)
            this->cacheDir = this->confDir / "cache";
        else
        {
            this->cacheDir = vm["cache"].as<std::string>();
            Tools::log << "Cache directory: " << this->cacheDir.string() << Tools::endl;
        }

        this->_ReadSettings();
    }

    void Settings::_ReadSettings()
    {
    }

}
