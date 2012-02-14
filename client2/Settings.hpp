#ifndef __CLIENT_SETTINGS_HPP__
#define __CLIENT_SETTINGS_HPP__

namespace Client {

    class Settings
    {
    public:
        std::string port;
        std::string host;
        boost::filesystem::path confdir;
        boost::filesystem::path bindingsFile;
        boost::filesystem::path settingsFile;
        unsigned int fps;
        unsigned int chunkCacheDistance;
        std::string nickname;

    public:
        Settings(int ac, char** av);
    private:
        void _ReadSettings();
    };

}

#endif
