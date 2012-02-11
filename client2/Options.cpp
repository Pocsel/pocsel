#include <boost/program_options.hpp>
#include <boost/cstdlib.hpp>

#include "client2/Options.hpp"
#include "ProgramInfo.hpp"

namespace Client {

    Options::Options(int ac, char** av)
    {
        boost::program_options::options_description options("Options");
        options.add_options()
            ("host", boost::program_options::value<std::string>()->default_value("localhost"), "Address of a " PROJECT_NAME " server")
            ("port", boost::program_options::value<std::string>()->default_value("8173"), "Port of the " PROJECT_NAME " server")
            ("confdir,c", boost::program_options::value<std::string>()->default_value("bite"), "Path to the client configuration directory")
            ("version,v", "Show version and exit")
            ("help,h", "Show this help message and exit");

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

//        this->worldFile = vm["world"].as<std::string>();
//        if (vm.count("host"))
//            ParseAddress(vm["host"].as<std::string>().c_str(), this->host, this->port);

        if (vm.count("host"))
        {
            Tools::log << "SJKDFJKSDKF " << vm["host"].as<std::string>() << Tools::endl;
        }
        if (vm.count("port"))
        {
            Tools::log << "SJKDFJKSDKF234 " << vm["port"].as<std::string>() << Tools::endl;
        }

        if (vm.count("help"))
        {
            Tools::log << "Usage: " << (ac > 0 && av[0] ? av[0] : PROGRAM_NAME) << " [host [port]] [-c CONFDIR] [-v] [-h]\n\n";
            Tools::log << options;
            exit(boost::exit_success);
        }

        if (vm.count("version"))
        {
            Tools::log << PROJECT_NAME << " ~ " << PROGRAM_NAME << " version " << GIT_VERSION << Tools::endl;
            exit(boost::exit_success);
        }
    }

}
