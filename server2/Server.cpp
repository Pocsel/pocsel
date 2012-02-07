#include <iostream>

#include <boost/asio/ip/address.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>

//#include "server/Game.hpp"
#include "server2/Server.hpp"
#include "server2/Settings.hpp"

namespace Server {

    Server::Server(int ac, char* av[]) :
        _ioService(),
        _acceptor(this->_ioService),
        _newConnection(),
        _settings(new Settings(ac, av))//,
//        _game(new Game(*this->_settings, this->_ioService))
    {
        try
        {
            boost::asio::ip::tcp::resolver resolver(this->_ioService);
            boost::asio::ip::tcp::resolver::query query(this->_settings->host,
                    Tools::ToString(this->_settings->port),
                    boost::asio::ip::tcp::resolver::query::passive);
            boost::asio::ip::tcp::endpoint endpoint =
                *(resolver.resolve(query)); // take first
            this->_acceptor.open(endpoint.protocol());
            this->_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
            this->_acceptor.bind(endpoint);
            this->_acceptor.listen();
            this->_ConnectAccept();
            std::cout << "Listening on " << endpoint.address().to_string()
                      << ':' << this->_settings->port << std::endl;
        }
        catch (std::exception& e)
        {
            std::cerr << "Cannot bind to '" << this->_settings->host << ':'
                      << this->_settings->port << "': " << e.what() << std::endl;
        }
    }

    Server::~Server()
    {
        Tools::Delete(this->_settings);
//        Tools::Delete(this->_game);
    }

    int Server::Run()
    {
        std::cout << "Running server2" << std::endl;
//        return this->_game->Run();
        return 0;
    }

    void Server::Stop()
    {
        std::cout << "Stopping server2" << std::endl;
//        this->_game->Stop();
        std::cout << "Stopping Network io_service" << std::endl;
        this->_ioService.stop();
    }

    void Server::_HandleAccept(boost::system::error_code const& e)
    {
        if (!e)
        {
//            this->_game->GetClientManager().HandleNewClient(this->_newConnection);
            this->_ConnectAccept();
//            std::cout << "New connection handled (total = "
//                      << this->_game->GetClientManager().GetNbClient() << ')' << std::endl;
        }
        else
        {
            std::cerr << "New connection has error !?" << std::endl;
        }
    }

    void Server::_ConnectAccept()
    {
        this->_newConnection.reset(new SocketType(this->_ioService));
        this->_acceptor.async_accept(
            *this->_newConnection,
            boost::bind(&Server::_HandleAccept, this, boost::asio::placeholders::error)
        );
    }

}
