#include <boost/program_options.hpp>
#include <boost/cstdlib.hpp>

#include "client/Settings.hpp"
#include "ProgramInfo.hpp"
#include "common/ConfDir.hpp"
#include "tools/lua/Interpreter.hpp"

namespace Client {

    Settings::Settings(int ac, char** av) :
        nickname("Player"),
        fps(60),
        chunkViewDistance(4),
        chunkCacheArea(2),
        chunkMinimumArea(3)
    {
        std::string defaultConfDir = Common::ConfDir::Client().string();

        boost::program_options::options_description options("Options");
        options.add_options()
            ("host", boost::program_options::value<std::string>()->default_value("localhost"),
             "Address of a " PROJECT_NAME " server")
            ("port", boost::program_options::value<std::string>()->default_value("8173"),
             "Port of the " PROJECT_NAME " server")
            ("conf,c", boost::program_options::value<std::string>()->default_value(defaultConfDir),
             "Path to a client configuration directory")
            ("settings,s", boost::program_options::value<std::string>(),
             "Path to a settings file")
            ("bindings,b", boost::program_options::value<std::string>(),
             "Path to a bindings file")
            ("cache", boost::program_options::value<std::string>(),
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

        if (vm.count("settings"))
        {
            this->settingsFile = vm["settings"].as<std::string>();
            Tools::log << "Settings file: " << this->settingsFile.string() << Tools::endl;
        }
        else
            this->settingsFile = this->confDir / "settings.lua";

        if (vm.count("bindings"))
        {
            this->bindingsFile = vm["bindings"].as<std::string>();
            Tools::log << "Bindings file: " << this->bindingsFile.string() << Tools::endl;
        }
        else
            this->bindingsFile = this->confDir / "bindings.lua";

        if (vm.count("cache"))
        {
            this->cacheDir = vm["cache"].as<std::string>();
            Tools::log << "Cache directory: " << this->cacheDir.string() << Tools::endl;
        }
        else
            this->cacheDir = this->confDir / "cache";

        this->_ReadSettings();
    }

    void Settings::_ReadSettings()
    {
        try
        {
            Tools::Lua::Interpreter i;
            i.ExecFile(this->settingsFile.string());

            this->nickname = i["nickname"].as<std::string>();
            this->fps = i["fps"].as<unsigned int>();
            if (this->fps < 2)
            {
                this->fps = 2;
                Tools::error << "Settings: fps too low, changing to " << this->fps << Tools::endl;
            }
            this->chunkViewDistance = i["chunkViewDistance"].as<unsigned int>();
            if (this->chunkViewDistance < 2)
            {
                this->chunkViewDistance = 2;
                Tools::error << "Settings: chunkViewDistance too low, changing to " << this->chunkViewDistance << Tools::endl;
            }
            this->chunkCacheArea = i["chunkCacheArea"].as<unsigned int>();
            if (this->chunkCacheArea < 1)
            {
                this->chunkCacheArea = 1;
                Tools::error << "Settings: chunkCacheArea too low, changing to " << this->chunkCacheArea << Tools::endl;
            }
            this->chunkMinimumArea = i["chunkMinimumArea"].as<unsigned int>();
            if (this->chunkMinimumArea < 1 || this->chunkMinimumArea > this->chunkViewDistance)
            {
                this->chunkMinimumArea = this->chunkViewDistance - 1;
                Tools::error << "Settings: invalid value for chunkMinimumArea, changing to " << this->chunkMinimumArea << Tools::endl;
            }
            this->res.x = i["resX"].as<unsigned int>();
            this->res.y = i["resY"].as<unsigned int>();
            this->fullscreen = i["fullscreen"].as<bool>();
            this->useShaders = i["useShaders"].as<bool>();
        }
        catch (std::exception& e)
        {
            Tools::error << "Failed to load settings file \"" << this->settingsFile.string() << "\": " << e.what() << Tools::endl;
            return;
        }
        Tools::debug << "Settings file \"" << this->settingsFile.string() << "\" successfully loaded.\n";
    }

}
