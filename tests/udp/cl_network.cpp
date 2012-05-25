#include "cl_network.hpp"
#include "cl_udppacket.hpp"
#include "cl_packetcreator.hpp"
#include "cl_packetextractor.hpp"
#include "tools/Deleter.hpp"

namespace cl {

    Network::Network() :
        _socket(_ioService),
        _udpSocket(_ioService),
        _thread(0),
        _sending(false),
        _isConnected(false),
        _isRunning(false),
        _loading(0),
        _sendingUdp(false)
    {
        this->_udpStatus.canReceive = false;
        this->_udpStatus.canSend = false;
        this->_udpStatus.serverCanReceive = false;
        this->_udpStatus.passThroughActive = false;
        this->_udpStatus.passThroughCount = 0;
        this->_udpStatus.ptOkSent = false;

        this->_sizeBuffer.resize(2);
        this->_udpDataBuffer.resize(Common::Packet::MaxSize);
    }

    float Network::GetLoadingProgression()
    {
        this->_loading += 0.00025f;
        return this->_loading;
    }

    void Network::Connect(std::string const& host, std::string const& port)
    {
        if (this->_isRunning)
            throw std::runtime_error("Network::Connect called while thread running");

        if (this->_thread) // dans le cas ou le _Connect() � foir�
        {
            this->_thread->join(); // on attend pour etre sur qu'il a completement return de _Connect()
            Tools::Delete(this->_thread); // et on ne leak pas de m�moire
        }

        this->_host = host;
        this->_port = port;
        this->_isRunning = true;
        this->_loading = 0;
        this->_lastError.clear();
        this->_thread = new boost::thread(std::bind(&Network::_Connect, this, this->_host, this->_port));
    }

    Network::~Network()
    {
        std::for_each(this->_outQueue.begin(), this->_outQueue.end(), [](Common::Packet* p) { Tools::Delete(p); });
        std::for_each(this->_outQueueUdp.begin(), this->_outQueueUdp.end(), [](UdpPacket* p) { Tools::Delete(p); });
        Tools::Delete(this->_thread);
    }

    void Network::_Connect(std::string host, std::string port)
    {
        try
        {
            boost::asio::ip::tcp::resolver resolver(this->_ioService);
            boost::asio::ip::tcp::resolver::query query(host, port);
            boost::asio::ip::tcp::resolver::iterator endpointIterator = resolver.resolve(query);
            boost::asio::ip::tcp::resolver::iterator end;
            boost::system::error_code error = boost::asio::error::host_not_found;
            while (error && endpointIterator != end)
            {
                this->_socket.close();
                this->_socket.connect(*endpointIterator++, error);
            }
            if (error)
            {
                Tools::error << "Network::Network: Connection to " << host << ":" << port << " failed: " << error.message() << ".\n";
                this->_isRunning = false;
                this->_lastError = error.message();
                return;
            }
            else
            {
                Tools::log << "Connection to " << host << ":" << port << " is a success.\n";
                this->_isConnected = true;
            }
        }
        catch (std::exception& e)
        {
            Tools::error << "Network::Network: Exception while connecting to " << host << ":" << port << ": " << e.what() << ".\n";
            this->_isRunning = false;
            this->_lastError = e.what();
            return;
        }

        try
        {
            boost::asio::ip::udp::endpoint endpoint(this->_socket.local_endpoint().address(), this->_socket.local_endpoint().port());
            this->_udpSocket.open(endpoint.protocol());
            this->_udpSocket.bind(endpoint);
            Tools::log << "Receiving on (UDP): " <<
                this->_socket.local_endpoint().address() <<
                ":" << this->_socket.local_endpoint().port() << "\n";

            this->_udpStatus.canReceive = true;

            boost::asio::ip::udp::endpoint sendEndpoint(this->_socket.remote_endpoint().address(), this->_socket.remote_endpoint().port());
            boost::system::error_code error = boost::asio::error::host_not_found;
            this->_udpSocket.connect(sendEndpoint, error);

            if (error)
            {
                Tools::error << "Network::Network: Connection to (UDP) " << host << ":" << port << " failed: " << error.message() << ".\n";
            }
            else
            {
                Tools::log << "Sending on (UDP): " <<
                    this->_socket.remote_endpoint().address() <<
                    ":" << this->_socket.remote_endpoint().port() << "\n";
                this->_udpStatus.canSend = true;
            }
        }
        catch (std::exception&)
        {
            Tools::error << "Exception while binding to (UDP) : " <<
                this->_socket.local_endpoint().address() <<
                ":" << this->_socket.local_endpoint().port() << "\n";
        }
        this->_Run();
    }

    void Network::PassThrough1()
    {
        std::cout << "PassThrough1()\n";
        if (this->_isConnected == false)
        {
            std::cout << "not connected at all\n";
            return;
        }
        if (this->_udpStatus.canSend == false)
        {
            std::cout << "cant send\n";
            return;
        }

        std::function<void(void)> fx =
            std::bind(&Network::PassThrough1, this);

        if (this->_udpStatus.passThroughActive == false)
        {
            std::cout << "count=" << this->_udpStatus.passThroughCount++ << "\n";
            if (this->_udpStatus.passThroughCount == 120)
            {
                std::cout << "too much\n";
                return;
            }
            this->_TimedDispatch(fx, 55);
        }
        else
        {
            this->_TimedDispatch(fx, 5555);
        }

        auto toto = PacketCreator::PassThrough(this->_id, 1);
        this->SendUdpPacket(std::move(toto));
    }

    void Network::_Run()
    {
        this->_ReceivePacketSize();
        auto toto = PacketCreator::UdpReady(this->_udpStatus.canReceive);
        this->_SendPacket(Tools::Deleter<Common::Packet>::CreatePtr(toto.release()));
        if (this->_udpStatus.canReceive)
            this->_ReceiveUdpPacket();
        this->_ioService.run();
    }

    void Network::Stop()
    {
        if (!this->_isRunning)
            throw std::runtime_error("Network::Connect called while thread not running");

        this->_ioService.stop();
        this->_thread->join();

        this->_ioService.reset();
        if (this->IsConnected())
            this->_CloseSocket();
        this->_isRunning = false;
        this->_lastError.clear();
        this->_isConnected = false;

        // Pas de leak dans le cas o� on refait un connect
        Tools::Delete(this->_thread);
        this->_thread = 0;
        std::for_each(this->_outQueue.begin(), this->_outQueue.end(), [](Common::Packet* p) { Tools::Delete(p); });
        this->_outQueue.clear();
        std::for_each(this->_outQueueUdp.begin(), this->_outQueueUdp.end(), [](UdpPacket* p) { Tools::Delete(p); });
        this->_outQueueUdp.clear();
    }

    void Network::SendPacket(std::unique_ptr<Common::Packet> packet)
    {
        std::function<void(void)> fx =
            std::bind(&Network::_SendPacket,
                      this,
                      Tools::Deleter<Common::Packet>::CreatePtr(packet.release()));
        this->_ioService.dispatch(fx);
    }

    void Network::_SendPacket(std::shared_ptr<Common::Packet> packet)
    {
        if (!this->_isConnected)
        {
            Tools::error << "Network::SendPacket: Sending packet with no open socket.\n";
            return;
        }

        this->_outQueue.push_back(Tools::Deleter<Common::Packet>::StealPtr(packet));
        if (!this->_sending)
        {
            this->_sending = true;
            this->_SendNext();
        }
    }

    void Network::SendUdpPacket(std::unique_ptr<UdpPacket> packet)
    {
        std::function<void(void)> fx =
            std::bind(&Network::_SendUdpPacket,
                      this,
                      Tools::Deleter<UdpPacket>::CreatePtr(packet.release()));
        this->_ioService.dispatch(fx);
    }

    void Network::_SendUdpPacket(std::shared_ptr<UdpPacket> packet)
    {
        if (!this->_isConnected)
        {
            Tools::error << "Network::SendPacket: Sending packet with no open socket.\n";
            return;
        }

        if (this->_udpStatus.canSend == false ||
                (packet->forceUdp == false && this->_udpStatus.serverCanReceive == false))
        {
            if (packet->forceUdp == true)
            {
                std::cout << "cant send this shit\n";
                return;
            }
            this->_SendPacket(Tools::Deleter<Common::Packet>::CreatePtr(Tools::Deleter<UdpPacket>::StealPtr(packet)));
            return;
        }

        this->_outQueueUdp.push_back(Tools::Deleter<UdpPacket>::StealPtr(packet));
        if (!this->_sendingUdp)
        {
            this->_sendingUdp = true;
            this->_SendNextUdp();
        }
    }

    void Network::_CloseSocket()
    {
        this->_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        this->_socket.close();

        this->_udpSocket.close();
    }

    void Network::_DisconnectedByNetwork(std::string const& error)
    {
        this->_isConnected = false;
        this->_lastError = error;

        try
        {
            this->_CloseSocket();
        }
        catch (std::exception& e)
        {
            Tools::error << "Network::_Disconnect: Exception on socket shutdown: \"" << e.what() << "\".\n";
            return;
        }
        Tools::log << "Network::_Disconnect: Socket disconnected.\n";
    }

    void Network::_SendNext()
    {
        std::cout << "_SendNext()\n";
        Common::Packet* p = this->_outQueue.front();
        std::vector<boost::asio::const_buffer> buffers;
        buffers.push_back(boost::asio::buffer(p->GetCompleteData(), p->GetCompleteSize()));
        boost::asio::async_write(this->_socket, buffers, boost::bind(&Network::_HandleWrite, this, boost::asio::placeholders::error));
    }

    void Network::_SendNextUdp()
    {
        UdpPacket* p = this->_outQueueUdp.front();
        std::vector<boost::asio::const_buffer> buffers;
        buffers.push_back(boost::asio::buffer(p->GetCompleteData(), p->GetCompleteSize()));
        this->_udpSocket.async_send(buffers, boost::bind(&Network::_HandleWriteUdp, this, boost::asio::placeholders::error));
    }

    void Network::_HandleWrite(boost::system::error_code const& error)
    {
        std::cout << "_HandleWrite()\n";
        if (error)
        {
            Tools::error << "Network::_HandleWrite: Write error: \"" << error.message() << "\".\n";
            this->_DisconnectedByNetwork(error.message());
        }
        else
        {
            Tools::Delete(this->_outQueue.front());
            this->_outQueue.pop_front();
            if (this->_outQueue.empty())
                this->_sending = false;
            else
                this->_SendNext();
        }
    }

    void Network::_HandleWriteUdp(boost::system::error_code const& error)
    {
        if (error)
        {
            Tools::error << "Network::_HandleWrite: UDP Write error: \"" << error.message() << "\".\n";
            this->_udpStatus.canSend = false;
            std::for_each(this->_outQueueUdp.begin(), this->_outQueueUdp.end(), [](UdpPacket* p) { Tools::Delete(p); });
            this->_outQueueUdp.clear();
        }
        else
        {
            Tools::Delete(this->_outQueueUdp.front());
            this->_outQueueUdp.pop_front();
            if (this->_outQueueUdp.empty())
                this->_sendingUdp = false;
            else
                this->_SendNextUdp();
        }
    }

    void Network::_ReceiveUdpPacket()
    {
        this->_udpSocket.async_receive_from(
            boost::asio::buffer(this->_udpDataBuffer),
            this->_udpSenderEndpoint,
            boost::bind(
                &Network::_HandleReceiveUdpPacket, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred)
            );
    }

    void Network::_HandleReceiveUdpPacket(boost::system::error_code const& error, std::size_t size)
    {
        if (error)
        {
            Tools::error << "Network::_HandleReceiveUdpPacket: Read error \"" << error.message() << "\".\n";
            this->_DisconnectedByNetwork(error.message());
        }
        else
        {
            std::cout << "udp packet received from " <<
                this->_udpSenderEndpoint.address() << ":" << this->_udpSenderEndpoint.port() << "\n";

            if (this->_udpStatus.canSend == true && this->_udpSenderEndpoint != this->_udpSocket.remote_endpoint())
            {
                std::cout << "packet received from the wrong endpoint, ignoring\n";
            }
            else
            {
                auto p = new Tools::ByteArray();
                p->SetData(this->_udpDataBuffer.data(), (Uint32)size);
                this->_HandlePacket(p);
            }
            this->_ReceiveUdpPacket();
        }
    }

    void Network::_ReceivePacketSize()
    {
        boost::asio::async_read(this->_socket, boost::asio::buffer(this->_sizeBuffer), boost::bind(&Network::_HandleReceivePacketSize, this, boost::asio::placeholders::error));
    }

    void Network::_HandleReceivePacketSize(const boost::system::error_code& error)
    {
        if (error)
        {
            Tools::error << "Network::_HandleReceivePacketSize: Read error: \"" << error.message() << "\".\n";
            this->_DisconnectedByNetwork(error.message());
        }
        else
            this->_ReceivePacketContent(ntohs(*reinterpret_cast<Uint16*>(this->_sizeBuffer.data())));
    }

    void Network::_ReceivePacketContent(unsigned int size)
    {
        this->_dataBuffer.resize(size);
        boost::asio::async_read(this->_socket, boost::asio::buffer(this->_dataBuffer), boost::bind(&Network::_HandleReceivePacketContent, this, boost::asio::placeholders::error));
    }

    void Network::_HandleReceivePacketContent(const boost::system::error_code& error)
    {
        if (error)
        {
            Tools::error << "Network::_HandleReceivePacketContent: Read error \"" << error.message() << "\".\n";
            this->_DisconnectedByNetwork(error.message());
        }
        else
        {
            auto p = new Tools::ByteArray();
            p->SetData(this->_dataBuffer.data(), (Uint32)this->_dataBuffer.size());
            this->_HandlePacket(p);
            this->_ReceivePacketSize();
        }
    }

    void Network::_HandlePacket(Tools::ByteArray* packet)
    {
        std::unique_ptr<Tools::ByteArray> _autoDelete(packet);

        std::cout << "Received packet!\n";

        try
        {
            tst_protocol::ActionType type;
            packet->Read(type);

            switch (type)
            {
                case (tst_protocol::ActionType)tst_protocol::ServerToClient::svLogin:
                {
                    Uint32 id;
                    PacketExtractor::Login(*packet, id);

                    this->_id = id;

                    this->PassThrough1();
                }
                break;
            case (tst_protocol::ActionType)tst_protocol::ServerToClient::svPassThrough:
                {
                    Uint32 ptType;
                    PacketExtractor::PassThrough(*packet, ptType);

                    this->_udpStatus.passThroughActive = true;

                    if (this->_udpStatus.ptOkSent == false)
                    {
                        auto toto = PacketCreator::PassThroughOk(ptType);
                        this->_SendPacket(Tools::Deleter<Common::Packet>::CreatePtr(toto.release()));
                        this->_udpStatus.ptOkSent = true;
                    }
                }
                break;
            case (tst_protocol::ActionType)tst_protocol::ServerToClient::svPassThroughOk:
                {
                    Uint32 ptType;
                    PacketExtractor::PassThroughOk(*packet, ptType);

                    switch (ptType)
                    {
                    case 1:
                        this->_udpStatus.serverCanReceive = true;
                        break;
                    default:
                        throw std::runtime_error("WTF unknown pass through type");
                    }
                }
                break;
            default:
                throw std::runtime_error("Unknown packet type (" + Tools::ToString(type) + ")");
            }
        }
        catch (std::exception& e)
        {
            std::cout << "could not read packet, serveur de merde: " << e.what() << "\n";
        }
    }

    void Network::_TimedDispatch(std::function<void(void)> fx, Uint32 ms)
    {
        boost::asio::deadline_timer* t = new boost::asio::deadline_timer(this->_ioService, boost::posix_time::milliseconds(ms));
        std::function<void(boost::system::error_code const& e)>
            function(std::bind(&Network::_ExecDispatch,
                               this,
                               fx,
                               std::shared_ptr<boost::asio::deadline_timer>(t),
                               std::placeholders::_1));
        t->async_wait(function);
    }

    void Network::_ExecDispatch(std::function<void(void)>& message,
            std::shared_ptr<boost::asio::deadline_timer>,
            boost::system::error_code const& error)
    {
        if (error == boost::asio::error::operation_aborted)
        {
            Tools::debug << "Timer aborted;";
            return;
        }

        message();
    }

}
