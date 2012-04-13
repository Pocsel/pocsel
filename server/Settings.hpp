#ifndef __SERVER_SETTINGS_HPP__
#define __SERVER_SETTINGS_HPP__

namespace Server {

    class Settings
    {
    public:
        boost::filesystem::path worldFile;
        boost::filesystem::path worldDir;
        std::string port;
        std::string udpPort;
        std::string host;
        std::string rconPort;
        std::string rconHost;

    public:
        Settings(int ac, char** av);
    };

}

#endif
