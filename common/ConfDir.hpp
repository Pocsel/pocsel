#ifndef __COMMON_CONFDIR_HPP__
#define __COMMON_CONFDIR_HPP__

#include "common/DevConfDir.hpp"

namespace Common { namespace ConfDir {

    boost::filesystem::path Base()
    {
        boost::filesystem::path path;
#ifdef DEBUG // en debug
        // chemin de dev généré par cmake
        path = DEV_CONF_DIR;
#else // en release
# ifdef _WIN32
        // Windows: %appdata%
# else
        // Linux: $HOME
        char const* home;
        char const* home2;
        if ((home = getenv("HOME")) || (home = getenv("USERPROFILE")))
            path = home;
        else if ((home = getenv("HOMEDRIVE")) && (home2 = getenv("HOMEPATH")))
            path = boost::filesystem::path(home) / home2;
        path / "." PROJECT_NAME;
# endif
        boost::filesystem::create_directory(path);
#endif
        return path;
    }

    boost::filesystem::path Client()
    {
        return Base() / "client";
    }

    boost::filesystem::path Server()
    {
        return Base() / "server";
    }
}}

#endif
