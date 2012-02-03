#include <boost/cstdlib.hpp>

#include "client2/Client.hpp"
#include "client2/WindowSdl.hpp"

namespace Client {

    Client::Client(int ac, char** av)
    {
        this->_window = new WindowSdl();
    }

    Client::~Client()
    {
        Tools::Delete(this->_window);
    }

    int Client::Run()
    {
        return boost::exit_success;
    }

}
