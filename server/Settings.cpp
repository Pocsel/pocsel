
#include <iostream>
#include <iomanip>

#include <boost/program_options.hpp>
// N'existe pas sous windows:
//extern "C" {
//#include <getopt.h>
//}

#include "tools/ToString.hpp"

#include "common/constants.hpp"

#include "Settings.hpp"

namespace po = boost::program_options;
using namespace Server;

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
            std::cerr << "Wrong port given (0), set to " << Common::DefaultPort << "\n";
            port = Common::DefaultPort;
        }
    }

} // !anonymous

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
        std::cerr << "Arguments error (try -h for help): " << ex.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    this->worldFile = vm["world"].as<std::string>();
    if (vm.count("host"))
        ParseAddress(vm["host"].as<std::string>().c_str(), this->host, this->port);

    if (vm.count("help"))
    {
        std::cout << "Usage: " << (ac > 0 ? av[0] : "server") << " [address] [OPTIONS]\n\n";
        std::cout << options << std::endl;
        exit(EXIT_SUCCESS);
    }

    if (vm.count("version"))
    {
        std::cout << "Trollcube 0.0.1 alpha build 1337 release candidate 42\n";
        exit(EXIT_SUCCESS);
    }

    //int c;
    //while ((c = getopt(ac, av, "hw:")) != -1)
    //{
    //  switch (c)
    //  {
    //  case 'w':
    //      this->worldFile = optarg;
    //      break;
    //  case 'h':
    //  case '?':
    //      Settings::PrintUsage(av[0]);
    //      if (c == 'h')
    //          Settings::PrintHelp();
    //      exit(EXIT_FAILURE);
    //  default:
    //      throw std::runtime_error("Unknown option flag '" + Tools::ToString<char>(c) + "'\n");
    //  }
    //}
    //if (optind < ac)
    //{
    //  if (ac - optind > 1)
    //  {
    //      std::cerr << "Too many arguments, try -h for help\n";
    //      exit(EXIT_FAILURE);
    //  }
    //  ParseAddress(av[optind], this->host, this->port);
    //}
}

//
//void Settings::PrintUsage(char const* progName)
//{
//  std::cout << "Usage: " << progName << " [address] [OPTIONS]\n";
//}
//
//void Settings::PrintHelp()
//{
//#define PAD(s)  "  " << std::setfill(' ') << std::setw(21) << std::left << s
//  std::cout << "\nServe a world file through a network interface.\n\n"
//            << "positional arguments:\n"
//            << PAD("address")
//            << "Address to listen to: '[host][:port]'.\n"
//            << PAD("") << "Examples of valid arguments:\n"
//            << PAD("") << "  Both host and port: '"<< Common::DefaultHost <<":8173' (default value)\n"
//            << PAD("") << "  Only the host: 'localhost' or '[::1]'\n"
//            << PAD("") << "  The port number ':"<< Common::DefaultPort <<"' or '8173' (greater than 0)\n"
//            << "\n"
//            << "optional arguments:\n"
//            << PAD("-h, --help") << "Show this help message and exit\n"
//            << PAD("-w, --world") << "Specify the world file to load\n"
//            << "\n";
//#undef PAD
//}
