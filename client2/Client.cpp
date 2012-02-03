#include <boost/cstdlib.hpp>

#include "client2/Client.hpp"

namespace Client {

    Client::Client(int ac, char** av)
    {
    }

    int Client::Run()
    {
        return boost::exit_success;
    }

}
