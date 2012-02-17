#ifndef __CLIENT_SETTINGS_HPP__
#define __CLIENT_SETTINGS_HPP__

#include "tools/Vector2.hpp"

namespace Client {

    class Settings
    {
    public:
        // CLI options
        std::string port;
        std::string host;
        boost::filesystem::path confDir;
        boost::filesystem::path bindingsFile;
        boost::filesystem::path settingsFile;
        boost::filesystem::path cacheDir;
        // settings.lua
        std::string nickname;
        unsigned int fps;
        Tools::Vector2u res;
        bool fullscreen;
        bool useShaders;
        unsigned int chunkViewDistance;
        unsigned int chunkCacheArea;
        unsigned int chunkMinimumArea;

    public:
        Settings(int ac, char** av);
    private:
        void _ReadSettings();
    };

}

#endif
