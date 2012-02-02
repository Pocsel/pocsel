#ifndef __SERVER_SERVER_HPP__
#define __SERVER_SERVER_HPP__

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/noncopyable.hpp>

#include "server/SocketPtr.hpp"

namespace Server
{
    class Game;
    class Settings;

    class Server : private boost::noncopyable
    {
    private:
        boost::asio::io_service         _ioService;
        boost::asio::ip::tcp::acceptor  _acceptor;
        SocketPtr                       _newConnection;
        Settings*                       _settings;
        Game*                           _game;

    public:
        Server(int ac, char *av[]);
        ~Server();
        int Run();
        void Stop();

    private:
        void _ConnectAccept();
        void _HandleAccept(boost::system::error_code const& e);
    };

}

#endif
