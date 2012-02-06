#ifndef  __SERVER_SETTINGS_HPP__
# define __SERVER_SETTINGS_HPP__

# include <string>

namespace Server {

    class Settings
    {
    public:
        std::string worldFile;
        std::string host;
        Uint16      port;

        Settings(int ac, char* av[]);

        //static void PrintUsage(char const* progName);
        //static void PrintHelp();
    };

}

#endif


