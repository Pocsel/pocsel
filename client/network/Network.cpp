#include "client/precompiled.hpp"

#include "client/network/Network.hpp"
#include "client/network/UdpPacket.hpp"
#include "client/Client.hpp"
#include "tools/logger/Logger.hpp"
#include "tools/ToString.hpp"
#include "common/Packet.hpp"

namespace Client { namespace Network {

    Network::Network() :
        _socket(_ioService),
        _udpSocket(_ioService),
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
        {
            boost::lock_guard<boost::mutex> lock(this->_metaMutex);
            if (this->_isRunning)
                throw std::runtime_error("Network::Connect called while thread running");
        }

        if (this->_thread) // dans le cas ou le _Connect() à foiré
        {
            this->_thread->join(); // on attend pour etre sur qu'il a completement return de _Connect()
            delete this->_thread; // et on ne leak pas de mémoire
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
        std::for_each(this->_outQueue.begin(), this->_outQueue.end(), [](Common::Packet* p) { delete p; });
        std::for_each(this->_inQueue.begin(), this->_inQueue.end(), [](Common::Packet* p) { delete p; });
        std::for_each(this->_outQueueUdp.begin(), this->_outQueueUdp.end(), [](UdpPacket* p) { delete p; });
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
                boost::lock_guard<boost::mutex> lock(this->_metaMutex);
                this->_isRunning = false;
                this->_lastError = error.message();
                return;
            }
            else
            {
                boost::lock_guard<boost::mutex> lock(this->_metaMutex);
                this->_isConnected = true;
            }
        }
        catch (std::exception& e)
        {
            Tools::error << "Network::Network: Exception while connecting to " << host << ":" << port << ": " << e.what() << ".\n";
            boost::lock_guard<boost::mutex> lock(this->_metaMutex);
            this->_isRunning = false;
            this->_lastError = e.what();
            return;
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
        this->_ioService.run();
    }

    void Network::Stop()
    {
        {
            boost::lock_guard<boost::mutex> lock(this->_metaMutex);
            if (!this->_isRunning)
                throw std::runtime_error("Network::Connect called while thread not running");
        }

        if (this->IsConnected())
            this->_CloseSocket();
        this->_ioService.stop();
        this->_ioService.reset();
        this->_thread->join();
        this->_isRunning = false;
        this->_lastError.clear();
        this->_isConnected = false;

        // Pas de leak dans le cas où on refait un connect
        delete this->_thread;
        this->_thread = 0;
        std::for_each(this->_outQueue.begin(), this->_outQueue.end(), [](Common::Packet* p) { delete p; });
        std::for_each(this->_inQueue.begin(), this->_inQueue.end(), [](Common::Packet* p) { delete p; });
        this->_outQueue.clear();
        this->_inQueue.clear();
        std::for_each(this->_outQueueUdp.begin(), this->_outQueueUdp.end(), [](UdpPacket* p) { delete p; });
        this->_outQueueUdp.clear();
    }

    void Network::SendPacket(std::unique_ptr<Common::Packet> packet)
    {
        {
            boost::lock_guard<boost::mutex> lock(this->_metaMutex);
            if (!this->_isConnected)
            {
                Tools::error << "Network::SendPacket: Sending packet with no open socket.\n";
                return;
            }
        }
        bool sendNext = false;
        {
            boost::lock_guard<boost::mutex> lock(this->_outMutex);
            this->_outQueue.push_back(packet.release());
            if (!this->_sending)
            {
                this->_sending = true;
                sendNext = true;
            }
        }
        if (sendNext)
            this->_SendNext();
    }

    void Network::SendUdpPacket(std::unique_ptr<UdpPacket> packet)
    {
        bool udp = false;
        {
            boost::lock_guard<boost::mutex> lock(this->_metaMutex);
            if (!this->_isConnected)
            {
                Tools::error << "Network::SendPacket: Sending packet with no open socket.\n";
                return;
            }
            udp = this->_udp;
        }
        if (!udp)
        {
            this->SendPacket(std::unique_ptr<Common::Packet>(packet.release()));
            return;
        }
        bool sendNext = false;
        {
            boost::lock_guard<boost::mutex> lock(this->_outMutexUdp);
            this->_outQueueUdp.push_back(packet.release());
            if (!this->_sendingUdp)
            {
                this->_sendingUdp = true;
                sendNext = true;
            }
        }
        if (sendNext)
            this->_SendNextUdp();
    }

    std::list<Common::Packet*> Network::GetInPackets()
    {
        boost::lock_guard<boost::mutex> lock(this->_inMutex);
        return std::move(this->_inQueue);
    }

    void Network::_CloseSocket()
    {
        this->_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        this->_socket.close();

        this->_udpSocket.close();
    }

    void Network::_DisconnectedByNetwork(std::string const& error)
    {
        {
            boost::lock_guard<boost::mutex> lock(this->_metaMutex);
            this->_isConnected = false;
            this->_lastError = error;
        }
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
        Common::Packet* p;
        {
            boost::lock_guard<boost::mutex> lock(this->_outMutex);
            p = this->_outQueue.front();
        }
        std::vector<boost::asio::const_buffer> buffers;
        buffers.push_back(boost::asio::buffer(p->GetCompleteData(), p->GetCompleteSize()));
        boost::asio::async_write(this->_socket, buffers, boost::bind(&Network::_HandleWrite, this, boost::asio::placeholders::error));
    }

    void Network::_SendNextUdp()
    {
        UdpPacket* p;
        {
            boost::lock_guard<boost::mutex> lock(this->_outMutexUdp);
            p = this->_outQueueUdp.front();
        }
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
            bool sendNext = false;
            {
                boost::lock_guard<boost::mutex> lock(this->_outMutex);
                Tools::Delete(this->_outQueue.front());
                this->_outQueue.pop_front();
                if (this->_outQueue.empty())
                    this->_sending = false;
                else
                    sendNext = true;
            }
            if (sendNext)
                this->_SendNext();
        }
    }

    void Network::_HandleWriteUdp(boost::system::error_code const& error)
    {
        if (error)
        {
            Tools::error << "Network::_HandleWrite: UDP Write error: \"" << error.message() << "\".\n";
            {
                boost::lock_guard<boost::mutex> lock(this->_metaMutex);
                this->_udp = false;
            }
            {
                boost::lock_guard<boost::mutex> lock(this->_outMutexUdp);
                std::for_each(this->_outQueueUdp.begin(), this->_outQueueUdp.end(), [](UdpPacket* p) { delete p; });
                this->_outQueueUdp.clear();
            }
        }
        else
        {
            bool sendNext = false;
            {
                boost::lock_guard<boost::mutex> lock(this->_outMutexUdp);
                Tools::Delete(this->_outQueueUdp.front());
                this->_outQueueUdp.pop_front();
                if (this->_outQueueUdp.empty())
                    this->_sendingUdp = false;
                else
                    sendNext = true;
            }
            if (sendNext)
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
            {
                boost::lock_guard<boost::mutex> lock(this->_inMutex);
                this->_inQueue.push_back(p);
            }
            this->_ReceivePacketSize();
        }
    }

}}
