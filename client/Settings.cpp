#include <boost/program_options.hpp>
#include <boost/cstdlib.hpp>

#include "client/Settings.hpp"
#include "ProgramInfo.hpp"
#include "common/ConfDir.hpp"
#include "tools/lua/Interpreter.hpp"

namespace {

    template <typename T>
        T _LuaGetGlobal(Tools::Lua::Interpreter const& i, std::string const& name, T const& defaultValue)
        {
            try
            {
                return i.Globals()[name].Check<T>();
            }
            catch (std::exception&)
            {
                Tools::error << "Settings: Could not read \"" << name << "\", using default \"" << defaultValue << "\"" << Tools::endl;
                return defaultValue;
            }
        }

}

namespace Client {

    Settings::Settings(int ac, char** av)
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
            i.DoFile(this->settingsFile.string());

            this->nickname = _LuaGetGlobal<std::string>(i, "nickname", "Newbie");
            this->fps = _LuaGetGlobal<unsigned int>(i, "fps", 60);
            if (this->fps < 2)
            {
                this->fps = 2;
                Tools::error << "Settings: fps too low, changing to " << this->fps << Tools::endl;
            }
            this->chunkViewDistance = _LuaGetGlobal<unsigned int>(i, "chunkViewDistance", 4);
            if (this->chunkViewDistance < 2)
            {
                this->chunkViewDistance = 2;
                Tools::error << "Settings: chunkViewDistance too low, changing to " << this->chunkViewDistance << Tools::endl;
            }
            this->chunkCacheArea = _LuaGetGlobal<unsigned int>(i, "chunkCacheArea", 10);
            if (this->chunkCacheArea < 1)
            {
                this->chunkCacheArea = 1;
                Tools::error << "Settings: chunkCacheArea too low, changing to " << this->chunkCacheArea << Tools::endl;
            }
            this->chunkMinimumArea = _LuaGetGlobal<unsigned int>(i, "chunkMinimumArea", 2);
            if (this->chunkMinimumArea < 1 || this->chunkMinimumArea > this->chunkViewDistance)
            {
                this->chunkMinimumArea = this->chunkViewDistance - 1;
                Tools::error << "Settings: invalid value for chunkMinimumArea, changing to " << this->chunkMinimumArea << Tools::endl;
            }
            this->res.x = _LuaGetGlobal<unsigned int>(i, "resX", 800);
            this->res.y = _LuaGetGlobal<unsigned int>(i, "resY", 600);
            this->fullscreen = _LuaGetGlobal<bool>(i, "fullscreen", false);
            this->useShaders = _LuaGetGlobal<bool>(i, "useShaders", true);
            this->mouseSensitivity = _LuaGetGlobal<float>(i, "mouseSensitivity", 0.707);
            if (this->mouseSensitivity > 1)
                this->mouseSensitivity = 1;
            else if (this->mouseSensitivity < 0)
                this->mouseSensitivity = 0;
        }
        catch (std::exception& e)
        {
            Tools::error << "Failed to load settings file \"" << this->settingsFile.string() << "\": " << e.what() << Tools::endl;
            return;
        }
        Tools::debug << "Settings file \"" << this->settingsFile.string() << "\" loaded.\n";
    }

}
