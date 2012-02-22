#include "server/network/Network.hpp"
#include "server/network/ClientConnection.hpp"

#include "server/Server.hpp"
#include "server/Settings.hpp"

namespace Server { namespace Network {

    Network::Network(Server& server, NewConnectionHandler& newConnectionHandler, UdpPacketHandler& udpPacketHandler) :
        _server(server),
        _newConnectionHandler(newConnectionHandler),
        _udpPacketHandler(udpPacketHandler),
        _ioService(),
        _acceptor(this->_ioService),
        _newConnection(0),
        _udpSocket(this->_ioService),
        _data(new Uint8[_buffSize])
    {
        Tools::debug << "Network::Network()\n";
        Settings const& settings = server.GetSettings();
        try
        {
            boost::asio::ip::tcp::resolver resolver(this->_ioService);
            boost::asio::ip::tcp::resolver::query query(
                    settings.host,
                    settings.port,
                    boost::asio::ip::tcp::resolver::query::passive
                    );
            boost::asio::ip::tcp::endpoint endpoint = *(resolver.resolve(query)); // take first
            this->_acceptor.open(endpoint.protocol());
            this->_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
            this->_acceptor.bind(endpoint);
            this->_acceptor.listen();
            this->_ConnectAccept();
            Tools::log <<
                "Listening on " << endpoint.address().to_string()
                << ":" << settings.port <<
                "\n";
        }
        catch (std::exception& e)
        {
            Tools::error <<
                "Cannot bind to '" << settings.host <<
                ":" << settings.port <<
                "': " << e.what() <<
                "\n";
            throw;
        }

        try
        {
            boost::asio::ip::udp::resolver udpResolver(this->_ioService);
            boost::asio::ip::udp::resolver::query udpQuery(
                    settings.host,
                    settings.udpPort,
                    boost::asio::ip::udp::resolver::query::passive
                    );
            boost::asio::ip::udp::endpoint udpEndpoint(*udpResolver.resolve(udpQuery)); // take first
            this->_udpSocket.open(udpEndpoint.protocol());
            this->_udpSocket.bind(udpEndpoint);

            this->_ConnectUdpRead();

            Tools::log <<
                "Listening on (UDP) " << udpEndpoint.address().to_string()
                << ":" << settings.udpPort <<
                "\n";
        }
        catch (std::exception& e)
        {
            Tools::error <<
                "Cannot bind to (UDP) '" << settings.host <<
                ":" << settings.udpPort <<
                "': " << e.what() <<
                "\n";
            throw;
        }
    }

    Network::~Network()
    {
        Tools::debug << "Network::~Network()\n";
        Tools::Delete(this->_newConnection);
        Tools::DeleteTab(this->_data);
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
            Tools::log << "New connection.\n";

            boost::shared_ptr<ClientConnection> newClientConnection(new ClientConnection(this->_newConnection));
            this->_newConnectionHandler(newClientConnection);
        }
        else
        {
            Tools::error << "New connection has error.\n";
            Tools::Delete(this->_newConnection);
        }

        this->_ConnectAccept();
    }

    void Network::_ConnectUdpRead()
    {
        this->_udpSocket.async_receive(
            boost::asio::buffer(this->_data, _buffSize),
            boost::bind(
                &Network::_HandleUdpRead, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred)
            );
    }

    void Network::_HandleUdpRead(boost::system::error_code const& e, std::size_t transferredBytes)
    {
        if (!e)
        {
            std::unique_ptr<Common::Packet> packet(new Common::Packet());
            packet->SetData((char*)this->_data, transferredBytes);
            this->_udpPacketHandler(packet);
        }
        else
        {
            Tools::error << "UPD read failure: " << e.message() << "\n";
        }

        this->_ConnectUdpRead();
    }

    void Network::Run()
    {
        Tools::debug << "Network::Run()\n";
        this->_ioService.run();
    }

    void Network::Stop()
    {
        Tools::debug << "Network::Stop()\n";
        this->_ioService.stop();
    }

}}
