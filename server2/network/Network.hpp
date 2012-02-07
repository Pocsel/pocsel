#ifndef __SERVER_NETWORK_NETWORK_HPP__
#define __SERVER_NETWORK_NETWORK_HPP__

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace Server {

    class Server;
    class Settings;

}

namespace Server { namespace Network {

    class ClientConnection;

    class Network :
        private boost::noncopyable
    {
    private:
        Server& _server;
        boost::asio::io_service _ioService;
        boost::asio::ip::tcp::acceptor _acceptor;
        boost::asio::ip::tcp::socket* _newConnection;

    public:
        Network(Server& server);
        ~Network();

        void Run();
        void Stop();
    private:
        void _ConnectAccept();
        void _HandleAccept(boost::system::error_code const& e);
    };

}}

#endif
