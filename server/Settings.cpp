#include <iomanip>
#include <boost/program_options.hpp>

#include "common/constants.hpp"

#include "server2/Settings.hpp"

namespace po = boost::program_options;

namespace {

    bool HasOnlyDigits(char const* s)
    {
        while (*s == '0')
            ++s;
        if (*s == '\0')
            return false;
        while (*s >= '0' && *s <= '9')
            ++s;
        return *s == '\0';
    }
    void ParseAddress(char const* addr, std::string& host, Uint16& port)
    {
        if (*addr == '\0')
            return;
        if (HasOnlyDigits(addr))
            port = atoi(addr);
        else if (*addr == ':' && HasOnlyDigits(addr + 1))
            port = atoi(addr + 1);
        else
        {
            size_t len = strlen(addr);
            assert(len > 0);
            size_t i = len - 1;
            while (i > 0 && addr[i] >= '0' && addr[i] <= '9')
                --i;
            if (i < len - 1 && (addr[i] == ':' || i == 0))
            {
                port = atoi(addr + i + 1);
                len = i;
            }
            while (len > 0 && addr[len - 1] == ':')
                --len;
            if (*addr && len > 0)
                host = std::string(addr, len);
            else if (port == 0)
            {
                port = Common::DefaultPort;
                host = "0";
            }
        }

        if (port == 0)
        {
            Tools::log << "Wrong port given (0), set to " << Common::DefaultPort << "\n";
            port = Common::DefaultPort;
        }
    }

}

namespace Server {

    Settings::Settings(int ac, char* av[]) :
        worldFile(Common::DefaultWorldFile),
        host(Common::DefaultHost),
        port(Common::DefaultPort)
    {
        po::options_description options("Options");
        options.add_options()
            ("version,v", "Print version string")
            ("help,h", "Show help message and exit")
            ("world,w", po::value<std::string>()->default_value(Common::DefaultWorldFile), "Specify the world file to load")
            ("host", po::value<std::string>()->default_value(std::string(Common::DefaultHost) + ":" + Tools::ToString(Common::DefaultPort)), "Address to listen to: '[host][:port]'");

        po::positional_options_description positional;
        positional.add("host", 1);

        po::variables_map vm;
        try
        {
            po::store(po::command_line_parser(ac, av).options(options).positional(positional).run(), vm);
            po::notify(vm);
        }
        catch (std::exception& ex)
        {
            Tools::error << "Arguments error (try -h for help): " << ex.what() << "\n";
            throw;
        }

        this->worldFile = vm["world"].as<std::string>();
        if (vm.count("host"))
            ParseAddress(vm["host"].as<std::string>().c_str(), this->host, this->port);

        if (vm.count("help"))
        {
            Tools::log << "Usage: " << (ac > 0 ? av[0] : "server2") << " [address] [OPTIONS]\n\n";
            Tools::log << options << "\n";
            exit(EXIT_SUCCESS);
        }

        if (vm.count("version"))
        {
            Tools::log << "Pocsel 0x00000001\n";
            exit(EXIT_SUCCESS);
        }
    }

}
