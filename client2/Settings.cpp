#include <boost/program_options.hpp>
#include <boost/cstdlib.hpp>

#include "client2/Settings.hpp"
#include "ProgramInfo.hpp"
#include "common/ConfDir.hpp"

namespace Client {

    Settings::Settings(int ac, char** av)
        : fps(60)
    {
        boost::program_options::options_description options("Options");
        options.add_options()
            ("host", boost::program_options::value<std::string>()->default_value("localhost"),
             "Address of a " PROJECT_NAME " server")
            ("port", boost::program_options::value<std::string>()->default_value("8173"),
             "Port of the " PROJECT_NAME " server")
            ("confdir,c", boost::program_options::value<std::string>()->default_value(Common::ConfDir::Client().string()),
             "Path to the client configuration directory")
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
            Tools::log << "Usage: " << (ac > 0 && av[0] ? av[0] : PROGRAM_NAME) << " [host [port]] [-c CONFDIR]\n\n";
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
        this->confdir = vm["confdir"].as<std::string>();
        Tools::debug << "Configuration directory: " << this->confdir << Tools::endl;
    }

}
