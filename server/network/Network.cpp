#include "server/network/Network.hpp"
#include "server/network/ClientConnection.hpp"

#include "server/Server.hpp"
#include "server/Settings.hpp"

#include "protocol/protocol.hpp"

namespace Server { namespace Network {

    Network::Network(Server& server,
                     NewConnectionHandler& newConnectionHandler,
                     PacketHandler& packetHandler,
                     ErrorHandler& errorHandler) :
        _server(server),
        _nextId(1),
        _newConnectionHandler(newConnectionHandler),
        _packetHandler(packetHandler),
        _errorHandler(errorHandler),
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
                    settings.port,
                    //settings.udpPort,
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

    void Network::SendUdpPacket(Uint32 connId)
    {
        this->_sendingConnections.push(connId);
        this->_ConnectUdpWrite();
    }

    void Network::RemoveConnection(Uint32 connId)
    {
        this->_connections.erase(connId);
        Tools::log << "Removed connection: " << connId << "\n";
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

            Uint32 newId = this->_GetNextId();
            boost::shared_ptr<ClientConnection> newClientConnection(
                new ClientConnection(
                    *this,
                    newId,
                    this->_newConnection,
                    this->_errorHandler,
                    this->_packetHandler)
                );
            this->_connections[newId] = newClientConnection;
            this->_newConnectionHandler(newId, newClientConnection);
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

    void Network::_ConnectUdpWrite()
    {
        // TODO verif si la udp socket elle est bien ok

        if (this->_sendingConnections.size() == 0 || this->_udpWriteConnected == true)
            return;

        this->_udpWriteConnected = true;

        while (this->_connections.count(this->_sendingConnections.front()) == 0)
        {
            this->_sendingConnections.pop();
            if (this->_sendingConnections.size() == 0)
                return;
        }

        boost::shared_ptr<ClientConnection>& conn = this->_connections.find(this->_sendingConnections.front())->second;

        std::unique_ptr<UdpPacket> packet = conn->GetUdpPacket();
        boost::asio::ip::udp::endpoint endpoint = conn->GetEndpoint();

        std::vector<boost::asio::const_buffer> buffers;
        buffers.push_back(boost::asio::buffer(packet->GetData(), packet->GetSize()));
        this->_udpSocket.async_send_to(
                buffers,
                endpoint,
                boost::bind(
                    &Network::_HandleUdpWrite,
                    this,
                    boost::shared_ptr<UdpPacket>(packet.release()),
                    boost::asio::placeholders::error)
                );
    }

    void Network::_HandleUdpWrite(boost::shared_ptr<UdpPacket> /*packetSent*/,
                                  boost::system::error_code const error)
    {
        this->_udpWriteConnected = false;
        if (!error)
        {
            this->_sendingConnections.pop();
            this->_ConnectUdpWrite();
        }
        else
        {
            // TODO
            //this->_sendingConnections.front().faireqqc();
            // TODO
            // réparer la socket udp
            this->_sendingConnections.pop();
            this->_ConnectUdpWrite();
        }
    }

    void Network::_HandleUdpRead(boost::system::error_code const& e, std::size_t transferredBytes)
    {
        if (!e)
        {
            if (transferredBytes < sizeof(Uint32) + sizeof(Protocol::ActionType))
            {
                Tools::error << "Received a too little packet\n";
            }
            else
            {
                Tools::ByteArray idArray;
                idArray.SetData((char*)this->_data, sizeof(Uint32));
                Uint32 id;
                idArray.Read(id);

                if (this->_connections.count(id) == 0)
                {
                    Tools::error << "Received packet for an unknown client\n";
                }
                else
                {
                    std::unique_ptr<Tools::ByteArray> packet(new Tools::ByteArray());
                    packet->SetData(((char*)this->_data) + sizeof(Uint32), (Uint16)(transferredBytes - sizeof(Uint32)));
                    this->_connections.find(id)->second->HandlePacket(packet);
                }
            }
        }
        else
        {
            Tools::error << "UDP read failure: " << e.message() << "\n";
        }

        this->_ConnectUdpRead();
    }

    Uint32 Network::_GetNextId()
    {
        while (this->_connections.count(this->_nextId) || this->_nextId == 0)
            ++this->_nextId;
        return this->_nextId++;
    }

}}
