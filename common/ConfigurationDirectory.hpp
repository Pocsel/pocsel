#ifndef __COMMON_CONFIGURATIONDIRECTORY_HPP__
#define __COMMON_CONFIGURATIONDIRECTORY_HPP__

namespace Common { namespace ConfigurationDirectory {

    boost::filesystem::path Get()
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

}}

#endif
