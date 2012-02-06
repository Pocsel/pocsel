#ifndef __CLIENT_SETTINGS_HPP__
#define __CLIENT_SETTINGS_HPP__

namespace Client {

    struct Settings
    {
        Settings(int ac, char** av);
        unsigned int fps;
        std::string host;
        std::string port;
    };

}

#endif
