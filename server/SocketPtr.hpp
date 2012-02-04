#ifndef __SOCKETPTR_HPP__
#define __SOCKETPTR_HPP__

#include <memory>
#include <boost/asio/ip/tcp.hpp>

namespace Server {

    typedef boost::asio::ip::tcp::socket SocketType;
    typedef std::unique_ptr<SocketType> SocketPtr;

}

#endif
