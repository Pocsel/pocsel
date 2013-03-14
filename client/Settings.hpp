#ifndef __CLIENT_SETTINGS_HPP__
#define __CLIENT_SETTINGS_HPP__

#include "tools/Vector2.hpp"

namespace Client {

    class Settings
    {
    public:
        // options CLI
        std::string port;
        std::string host;
        boost::filesystem::path confDir;
        boost::filesystem::path bindingsFile;
        boost::filesystem::path settingsFile;
        boost::filesystem::path cacheDir;
        // settings.lua
        std::string nickname;
        unsigned int fps;
        glm::uvec2 res;
        bool fullscreen;
        bool useDirect3D9;
        unsigned int chunkViewDistance;
        unsigned int chunkCacheArea;
        unsigned int chunkMinimumArea;
        float mouseSensitivity;
        bool drawPhysics;

    public:
        Settings(int ac, char** av);
    private:
        void _ReadSettings();
    };

}

#endif
