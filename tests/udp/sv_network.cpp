#include "sv_network.hpp"
#include "sv_connection.hpp"
#include "sv_packetcreator.hpp"
#include "common/Packet.hpp"


namespace sv {

    std::string const Network::Settings::host = "0.0.0.0";
    std::string const Network::Settings::port = "8173";

    Network::Network() :
        _ioService(),
        _acceptor(this->_ioService),
        _newConnection(0),
        _udpSocket(this->_ioService),
        _data(new Uint8[_buffSize]),
        _nextId(1),
        _udpWriteConnected(false)
    {
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
                    boost::asio::ip::udp::resolver::query::passive
                    );
            boost::asio::ip::udp::endpoint udpEndpoint(*udpResolver.resolve(udpQuery)); // take first
            this->_udpSocket.open(udpEndpoint.protocol());
            this->_udpSocket.bind(udpEndpoint);

            this->_ConnectUdpRead();

            Tools::log <<
                "Listening on (UDP) " << udpEndpoint.address().to_string()
                << ":" << settings.port <<
                "\n";
        }
        catch (std::exception& e)
        {
            Tools::error <<
                "Cannot bind to (UDP) '" << settings.host <<
                ":" << settings.port <<
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

    void Network::IHasPacketToSend(boost::shared_ptr<Connection> conn)
    {
        this->_sendingConnections.push(conn);
        this->_ConnectUdpWrite();
    }

    void Network::RemoveConnection(boost::shared_ptr<Connection> conn)
    {
        for (auto it = this->_connections.begin(), ite = this->_connections.end(); it != ite; ++it)
        {
            if (it->second == conn)
            {
                this->_connections.erase(it);
                std::cout << "removed: " << it->first << "\n";
                return;
            }
        }
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

            boost::shared_ptr<Connection> newConnection(new Connection(*this, this->_newConnection));
            this->_HandleNewConnection(newConnection);
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
        this->_udpSocket.async_receive_from(
            boost::asio::buffer(this->_data, _buffSize),
            this->_udpSenderEndpoint,
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
            std::cout << "udp packet received from " <<
                this->_udpSenderEndpoint.address() << ":" << this->_udpSenderEndpoint.port() << "\n";

            std::unique_ptr<Tools::ByteArray> packet(new Tools::ByteArray());
            packet->SetData((char*)this->_data, (Uint16)transferredBytes);
            this->_HandleUdpPacket(packet);
        }
        else
        {
            Tools::error << "UDP read failure: " << e.message() << "\n";
        }

        this->_ConnectUdpRead();
    }

    void Network::_ConnectUdpWrite()
    {
        // TODO verif si la udp socket elle est bien ok

        std::cout << "_connectudpwrite0\n";
        if (this->_sendingConnections.size() == 0 || this->_udpWriteConnected == true)
            return;

        this->_udpWriteConnected = true;

        std::unique_ptr<Common::Packet> packet = this->_sendingConnections.front()->GetUdpPacket();
        boost::asio::ip::udp::endpoint endpoint = this->_sendingConnections.front()->GetEndpoint();

        std::cout << "Sending udp packet to " << endpoint.address() << ":" << endpoint.port() << "\n";

        std::vector<boost::asio::const_buffer> buffers;
        buffers.push_back(boost::asio::buffer(packet->GetData(), packet->GetSize()));
        this->_udpSocket.async_send_to(
                buffers,
                endpoint,
                boost::bind(
                    &Network::_HandleUdpWrite,
                    this,
                    boost::shared_ptr<Common::Packet>(packet.release()),
                    boost::asio::placeholders::error)
                );
        std::cout << "connectudpwrite\n";
    }

    void Network::_HandleUdpWrite(boost::shared_ptr<Common::Packet> /*packetSent*/,
                                           boost::system::error_code const error)
    {
        this->_udpWriteConnected = false;
        if (!error)
        {
            std::cout << "_handleudpwrite ok\n";
            this->_sendingConnections.pop();
            this->_ConnectUdpWrite();
        }
        else
        {
            //this->_sendingConnections.front().faireqqc();
            this->_sendingConnections.pop();
            this->_ConnectUdpWrite();
            std::cout << "_HANDLEUDPWRITE error\n";
            // this->_udp = false;
        }
    }

    void Network::_HandleNewConnection(boost::shared_ptr<Connection> conn)
    {
        Uint32 id = this->_nextId++;

        std::cout << "New client: " << id << "\n";

        this->_connections[id] = conn;

        conn->ConnectRead();

        auto toto = PacketCreator::Login(id);
        conn->SendPacket(toto);
    }

    void Network::_HandleUdpPacket(std::unique_ptr<Tools::ByteArray>& packet)
    {
        Uint32 id;
        try
        {
            packet->Read(id);
        }
        catch (std::exception& e)
        {
            std::cout << "ERROR: could not read udp packet\n";
            return;
        }

        auto it = this->_connections.find(id);
        if (it == this->_connections.end())
        {
            std::cout << "udp packet received: unknown id: " << id << "\n";
            return;
        }

        std::cout << "UDP packet received: " << id << "\n";
        it->second->SetEndpoint(this->_udpSenderEndpoint);
        it->second->HandlePacket(packet);
    }
}
