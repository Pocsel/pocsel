#include "client/precompiled.hpp"

#include "client/network/Network.hpp"
#include "client/network/UdpPacket.hpp"
#include "client/Client.hpp"
#include "tools/logger/Logger.hpp"
#include "tools/ToString.hpp"
#include "tools/Deleter.hpp"

namespace Client { namespace Network {

    Network::Network() :
        _socket(_ioService),
        _udpSocket(_ioService),
        _udpReceiveSocket(_ioService),
        _thread(0),
        _sending(false),
        _isConnected(false),
        _isRunning(false),
        _loading(0),
        _sendingUdp(false),
        _udp(false)
    {
        this->_sizeBuffer.resize(2);
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

        if (this->_thread) // dans le cas ou le _Connect() à foiré
        {
            this->_thread->join(); // on attend pour etre sur qu'il a completement return de _Connect()
            Tools::Delete(this->_thread); // et on ne leak pas de mémoire
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
            boost::system::error_code error = boost::asio::error::host_not_found;
            this->_udpReceiveSocket.open(endpoint.protocol());
            this->_udpReceiveSocket.bind(endpoint);
            if (error)
            {
                Tools::error << "Network::Network: Receiving to (UDP) " << host << ":" << port << " failed: " << error.message() << ".\n";
                this->_udpReceive = false;
            }
            else
            {
                this->_udpReceive = true;
            }
        }
        catch (std::exception& e)
        {
            Tools::error << "Network::Network: Exception while Receiving to (UDP) " << host << ":" << port << ": " << e.what() << ".\n";
            this->_udpReceive = false;
        }

        try
        {
            boost::asio::ip::udp::endpoint endpoint(this->_socket.remote_endpoint().address(), this->_socket.remote_endpoint().port());
            boost::system::error_code error = boost::asio::error::host_not_found;
            this->_udpSocket.connect(endpoint, error);
            if (error)
            {
                Tools::error << "Network::Network: Connection to (UDP) " << host << ":" << port << " failed: " << error.message() << ".\n";
                this->_udp = false;
            }
            else
            {
                this->_udp = true;
            }
        }
        catch (std::exception& e)
        {
            Tools::error << "Network::Network: Exception while connecting to (UDP) " << host << ":" << port << ": " << e.what() << ".\n";
            this->_udp = false;
        }
        this->_Run();
    }

    void Network::_Run()
    {
        this->_ReceivePacketSize();
//        if (this->_udpReceive)
//            this->_ReceiveUdpPacket();
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

        // Pas de leak dans le cas où on refait un connect
        Tools::Delete(this->_thread);
        this->_thread = 0;
        std::for_each(this->_outQueue.begin(), this->_outQueue.end(), [](Common::Packet* p) { Tools::Delete(p); });
        this->_inQueue.Clear();
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

        if (!this->_udp)
        {
            this->_SendPacket(std::shared_ptr<Common::Packet>(Tools::Deleter<UdpPacket>::StealPtr(packet)));
            return;
        }

        this->_outQueueUdp.push_back(Tools::Deleter<UdpPacket>::StealPtr(packet));
        if (!this->_sendingUdp)
        {
            this->_sendingUdp = true;
            this->_SendNextUdp();
        }
    }

    std::list<Common::Packet*> Network::GetInPackets()
    {
        return this->_inQueue.StealPackets();
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
            this->_udp = false;
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
            auto p = new Common::Packet();
            p->SetData(this->_dataBuffer.data(), (Uint16)this->_dataBuffer.size());
            this->_inQueue.PushPacket(p);
            this->_ReceivePacketSize();
        }
    }

}}
