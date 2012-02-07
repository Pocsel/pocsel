#include "server2/network/Network.hpp"
#include "server2/network/ClientConnection.hpp"

#include "server2/Server.hpp"
#include "server2/Settings.hpp"

#include "server2/clientmanagement/ClientManager.hpp"


namespace Server { namespace Network {

    Network::Network(Server& server) :
        _server(server),
        _ioService(),
        _acceptor(this->_ioService),
        _newConnection()
    {
        Settings const& settings = server.GetSettings();
        try
        {
            boost::asio::ip::tcp::resolver resolver(this->_ioService);
            boost::asio::ip::tcp::resolver::query query(
                    settings.host,
                    Tools::ToString(settings.port),
                    boost::asio::ip::tcp::resolver::query::passive
                    );
            boost::asio::ip::tcp::endpoint endpoint = *(resolver.resolve(query)); // take first
            this->_acceptor.open(endpoint.protocol());
            this->_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
            this->_acceptor.bind(endpoint);
            this->_acceptor.listen();
            this->_ConnectAccept();
            std::cout <<
                "Listening on " << endpoint.address().to_string()
                << ":" << settings.port <<
                "\n";
        }
        catch (std::exception& e)
        {
            std::cerr <<
                "Cannot bind to '" << settings.host <<
                ":" << settings.port <<
                "': " << e.what() <<
                "\n";
        }
    }

    Network::~Network()
    {
    }

    void Network::_ConnectAccept()
    {
        this->_newConnection = new boost::asio::ip::tcp::socket(this->_ioService);
        this->_acceptor.async_accept(
            *this->_newConnection,
            boost::bind(&Network::_HandleAccept, this, boost::asio::placeholders::error)
        );
    }

    void Network::_HandleAccept(boost::system::error_code const& e)
    {
        if (!e)
        {
            std::cout << "New connection.\n";
            ClientConnection* newClientConnection = new ClientConnection(this->_newConnection, this->_server.GetClientManager());
            this->_server.GetClientManager().HandleNewClient(newClientConnection);
            this->_ConnectAccept();
        }
        else
        {
            std::cerr << "New connection has error !?" << std::endl;
        }
    }

    void Network::Run()
    {
        std::cout << "Network::Run()\n";
        this->_ioService.run();
    }

    void Network::Stop()
    {
        std::cout << "Network::Stop()\n";
        this->_ioService.stop();
    }

}}
