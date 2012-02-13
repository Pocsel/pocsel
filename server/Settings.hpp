#ifndef __SERVER_SETTINGS_HPP__
#define __SERVER_SETTINGS_HPP__

namespace Server {

    class Settings
    {
    public:
        std::string worldFile;
        std::string host;
        Uint16 port;

    public:
        Settings(int ac, char* av[]);
    };

}

#endif
