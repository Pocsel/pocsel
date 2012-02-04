#ifndef __SERVER_CLIENTPTR_HPP__
#define __SERVER_CLIENTPTR_HPP__

#include <boost/shared_ptr.hpp>

namespace Server {

    class Client;

    typedef boost::shared_ptr<Client> ClientPtr;

}

#endif
