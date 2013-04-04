#include <boost/program_options.hpp>
#include <boost/cstdlib.hpp>
#include <luasel/Luasel.hpp>

#include "server/Settings.hpp"
#include "ProgramInfo.hpp"
#include "common/ConfDir.hpp"
#include "common/constants.hpp"

namespace Server {

    Settings::Settings(int ac, char** av) :
        // initialisation pour les seules variables ne prenant pas de valeur par défaut dans la lecture des settings/CLI/rcon
        debug(false)
    {
        boost::program_options::options_description options("Options");
        options.add_options()
            ("world,w", boost::program_options::value<std::string>()->default_value(Common::ConfDir::Server().string() + "/default_world/default_world." + Common::WorldFileExt),
             "Path to a " PROJECT_NAME " world file")
            ("settings,s", boost::program_options::value<std::string>(),
             "Path to a settings file")
            ("port,p", boost::program_options::value<std::string>()->default_value("8173"),
             "On which port to listen")
            ("udpport,u", boost::program_options::value<std::string>()->default_value("8173"),
             "On which UDP port to listen")
            ("host", boost::program_options::value<std::string>()->default_value("0.0.0.0"),
             "On which address to bind")
            ("rconsettings,r", boost::program_options::value<std::string>(),
             "Path to an rcon settings file")
            ("rconport", boost::program_options::value<std::string>()->default_value("8174"),
             "On which port to listen for rcon administration")
            ("rconhost", boost::program_options::value<std::string>()->default_value("0.0.0.0"),
             "On which address to bind for rcon administration")
            ("version,v",
             "Show version and exit")
            ("help,h",
             "Show this help message and exit");

        boost::program_options::positional_options_description positional;
        positional.add("world", 1);

        boost::program_options::variables_map vm;
        try
        {
            boost::program_options::store(boost::program_options::command_line_parser(ac, av).options(options).positional(positional).run(), vm);
            boost::program_options::notify(vm);
        }
        catch (std::exception& e)
        {
            Tools::error << "Invalid arguments (use -h for help): " << e.what() << std::endl;
            exit(boost::exit_failure);
        }

        // info + exit
        if (vm.count("help"))
        {
            Tools::log << "Usage: " << (ac > 0 && av[0] ? av[0] : PROGRAM_NAME) << " [world_dir] [-p PORT] [--host HOST] [-u udpPORT]\n\n";
            Tools::log << options;
            exit(boost::exit_success);
        }
        if (vm.count("version"))
        {
            Tools::log << PROJECT_NAME << " - " << PROGRAM_NAME << " - Version " << GIT_VERSION << std::endl;
            exit(boost::exit_success);
        }

        // CLI parameters
        this->port = vm["port"].as<std::string>();
        this->udpPort = vm["udpport"].as<std::string>();
        this->host = vm["host"].as<std::string>();
        this->rconPort = vm["rconport"].as<std::string>();
        this->rconHost = vm["rconhost"].as<std::string>();
        this->worldFile = vm["world"].as<std::string>();
        this->worldDir = this->worldFile.parent_path();
        Tools::log << "World directory: " << this->worldDir.string() << std::endl;

        // settings.lua
        if (vm.count("settings"))
            this->settingsFile = vm["settings"].as<std::string>();
        else
            this->settingsFile = this->worldDir / "settings.lua";
        Tools::log << "Settings file: " << this->settingsFile.string() << std::endl;
        this->_ReadSettings();

        // rcon.lua
        if (vm.count("rconsettings"))
            this->rconSettingsFile = vm["rconsettings"].as<std::string>();
        else
            this->rconSettingsFile = this->worldDir / "rcon.lua";
        Tools::log << "Rcon settings file: " << this->rconSettingsFile.string() << std::endl;
        this->_ReadRconSettings();
    }

    void Settings::_ReadSettings()
    {
        try
        {
            Luasel::Interpreter i;
            i.DoFile(this->settingsFile.string());

            // debug
            auto debugRef = i.Globals()["debug"];
            if (debugRef.IsBoolean())
            {
                this->debug = debugRef.ToBoolean();
                if (this->debug)
                    Tools::log << "Server running in debug mode!" << std::endl;
            }
            else if (debugRef.Exists())
                Tools::error << "Settings: Error: \"debug\" must be a boolean." << std::endl;
        }
        catch (std::exception& e)
        {
            Tools::error << "Failed to load settings file \"" << this->settingsFile.string() << "\": " << e.what() << std::endl;
            return;
        }
        Tools::debug << "Settings file \"" << this->settingsFile.string() << "\" loaded." << std::endl;
    }

    void Settings::_ReadRconSettings()
    {
        try
        {
            Luasel::Interpreter i;
            i.DoFile(this->rconSettingsFile.string());

            // rconUsers
            auto rconUsers = i.Globals()["rconUsers"];
            if (rconUsers.IsTable())
            {
                auto it = rconUsers.Begin();
                auto itEnd = rconUsers.End();
                for (; it != itEnd; ++it)
                {
                    auto loginRef = it.GetKey();
                    auto userRef = it.GetValue();
                    if (loginRef.IsString() && loginRef.GetLength() >= 1 && userRef.IsTable())
                    {
                        std::string login = loginRef.ToString();
                        RconUser user;
                        if (userRef["password"].IsString())
                            user.password = userRef["password"].ToString();
                        else if (userRef["password"].Exists())
                            Tools::error << "Rcon settings: Error: \"rconUsers\": User \"" << login << "\" has an invalid password." << std::endl;
                        if (userRef["rights"].IsTable())
                        {
                            auto itRights = userRef["rights"].Begin();
                            auto itRightsEnd = userRef["rights"].End();
                            for (; itRights != itRightsEnd; ++itRights)
                                if (itRights.GetValue().IsString())
                                    user.rights.push_back(itRights.GetValue().ToString());
                                else
                                    Tools::error << "Rcon settings: Error: \"rconUsers\": Right \"" << itRights.GetValue().ToString() << "\" for user \"" << login << "\" is invalid." << std::endl;
                        }
                        else if (userRef["rights"].Exists())
                            Tools::error << "Rcon settings: Error: \"rconUsers\": User \"" << login << "\" has an rights table." << std::endl;
                        this->rconUsers[login] = user;
                    }
                    else
                        Tools::error << "Rcon settings: Error: \"rconUsers\": Invalid user \"" << loginRef.ToString() << "\"." << std::endl;
                }
            }
            else if (rconUsers.Exists())
                Tools::error << "Rcon settings: Error: \"rconUsers\" must be a table." << std::endl;
        }
        catch (std::exception& e)
        {
            Tools::error << "Failed to load rcon settings file \"" << this->rconSettingsFile.string() << "\": " << e.what() << std::endl;
            return;
        }
        Tools::debug << "Rcon settings file \"" << this->rconSettingsFile.string() << "\" loaded." << std::endl;

        this->_DumpRconUsers();
    }

    void Settings::_DumpRconUsers() const
    {
        Tools::debug << this->rconUsers.size() << " rcon users:" << std::endl;
        auto it = this->rconUsers.begin();
        auto itEnd = this->rconUsers.end();
        for (; it != itEnd; ++it)
        {
            Tools::debug << " - \"" << it->first << "\" - Password: ";
            if (it->second.password.empty())
                Tools::debug << "none (account inactive)";
            else
                for (unsigned int i = 0; i < it->second.password.size(); ++i)
                    Tools::debug << "*";
            Tools::debug << " - Rights: ";
            auto itRights = it->second.rights.begin();
            auto itRightsEnd = it->second.rights.end();
            for (; itRights != itRightsEnd; ++itRights)
                Tools::debug << *itRights << " ";
            Tools::debug << "" << std::endl;
        }
    }

}
