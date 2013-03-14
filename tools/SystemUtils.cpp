#include "tools/precompiled.hpp"

#ifdef __gnu_linux__
# include <unistd.h>
#endif

#include "SystemUtils.hpp"

namespace Tools {

// cf http://stackoverflow.com/questions/1023306/finding-current-executables-path-without-proc-self-exe
#ifdef _WIN32
    std::string GetCurrentExecutablePath()
    {
        char buffer[1024 * 33]; // 1ko en plus au cas ou - pas de macro pour la taille max a cause de l'unicode...
        DWORD len;
        if ((len = GetModuleFileName(nullptr, buffer, sizeof(buffer))) == 0)
            throw std::runtime_error("GetCurrentExecutablePath");
        return std::string(buffer, len);
    }
#elif __gnu_linux__
    std::string GetCurrentExecutablePath()
    {
        char buffer[PATH_MAX + 1024]; // 1ko en plus au cas ou
        ssize_t len;
        if ((len = readlink("/proc/self/exe", buffer, sizeof(buffer))) == -1)
            throw std::runtime_error("GetCurrentExecutablePath");
        return std::string(buffer, len);
    }
#endif

}