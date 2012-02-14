#include <boost/program_options.hpp>
#include <boost/cstdlib.hpp>

#include "server/Settings.hpp"
#include "ProgramInfo.hpp"
#include "common/ConfDir.hpp"

namespace Server {

    Settings::Settings(int ac, char** av)
    {
        boost::program_options::options_description options("Options");
        options.add_options()
            ("world,w", boost::program_options::value<std::string>()->default_value(Common::ConfDir::Server().string() + "/default_world/default_world.tcworld"),
             "Path to a " PROJECT_NAME " world file")
            ("port,p", boost::program_options::value<std::string>()->default_value("8173"),
             "On which port to listen")
            ("host", boost::program_options::value<std::string>()->default_value("0"),
             "On which address to bind")
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
            Tools::error << "Invalid arguments (use -h for help): " << e.what() << Tools::endl;
            exit(boost::exit_failure);
        }

        if (vm.count("help"))
        {
            Tools::log << "Usage: " << (ac > 0 && av[0] ? av[0] : PROGRAM_NAME) << " [world_dir] [-p PORT] [--host HOST]\n\n";
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
        this->worldFile = vm["world"].as<std::string>();
        this->worldDir = this->worldFile.parent_path();
        Tools::log << "World directory: " << this->worldDir.string() << Tools::endl;
    }

}
