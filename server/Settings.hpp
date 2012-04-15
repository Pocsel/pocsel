#ifndef __SERVER_SETTINGS_HPP__
#define __SERVER_SETTINGS_HPP__

namespace Server {

    class Settings
    {
    public:
        struct RconUser
        {
            std::string password;
            std::list<std::string> rights;
        };

    public:
        // options CLI
        boost::filesystem::path worldFile;
        boost::filesystem::path worldDir; // trouvé a partir de worldFile
        boost::filesystem::path settingsFile;
        std::string port;
        std::string udpPort;
        std::string host;
        std::string rconPort;
        std::string rconHost;
        // settings.lua (trouvé dans worldDir par défaut)
        std::map<std::string /* login */, RconUser> rconUsers;

    public:
        Settings(int ac, char** av);
    private:
        void _ReadSettings();
        void _DumpRconUsers() const;
    };

}

#endif
