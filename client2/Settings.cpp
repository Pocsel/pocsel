#include "client2/Settings.hpp"

namespace Client {

    Settings::Settings(int ac, char** av)
    {
        this->fps = 60;
        this->host = "localhost";
        this->port = "8173";
    }

}
