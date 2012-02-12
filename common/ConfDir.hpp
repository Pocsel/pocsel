#ifndef __COMMON_CONFDIR_HPP__
#define __COMMON_CONFDIR_HPP__

namespace Common { namespace ConfDir {

    boost::filesystem::path Base()
    {
        boost::filesystem::path path;
#ifdef DEBUG
        // chemin de dev généré par cmake
        path = "test";
#else
# ifdef _WIN32
        // Windows: %appdata%
# else
        // Linux: $HOME
# endif
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
