#include <iostream>

#include "client2/network/Network.hpp"
#include "client2/Client.hpp"
#include "tools/logger/Logger.hpp"
#include "tools/ToString.hpp"
#include "common/Packet.hpp"

namespace Client { namespace Network {

    Network::Network(Client& client)
        : _socket(_ioService),
        _thread(0),
        _sending(false),
        _isConnected(false)
    {
        this->_sizeBuffer.resize(2);
    }

    void Network::Connect(std::string const& host, std::string const& port)
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
            Tools::error << "Network::Network: Connection to " << host << ":" << port << " failed.\n";
            throw std::runtime_error("connection failed");
        }
        this->_host = host;
        this->_port = port;
        this->_isConnected = true;
        this->_thread = new boost::thread(std::bind(&Network::Run, this));
    }

    Network::~Network()
    {
        std::for_each(this->_outQueue.begin(), this->_outQueue.end(), [](Common::Packet* p) { delete p; });
        std::for_each(this->_inQueue.begin(), this->_inQueue.end(), [](Common::Packet* p) { delete p; });
        Tools::Delete(this->_thread);
    }

    bool Network::IsConnected() const
    {
        return this->_isConnected;
    }

    std::string const& Network::GetHost() const
    {
        return this->_host;
    }

    std::string const& Network::GetPort() const
    {
        return this->_port;
    }

    void Network::Run()
    {
        this->_ReceivePacketSize();
        this->_ioService.run();
    }

    void Network::Stop()
    {
        this->_isConnected = false;
        this->_ioService.stop();
        this->_thread->join();

        // Pas de leak dans le cas où on refait un connect
        delete this->_thread;
        this->_thread = 0;
        std::for_each(this->_outQueue.begin(), this->_outQueue.end(), [](Common::Packet* p) { delete p; });
        std::for_each(this->_inQueue.begin(), this->_inQueue.end(), [](Common::Packet* p) { delete p; });
        this->_outQueue.clear();
        this->_inQueue.clear();
    }

    void Network::SendPacket(std::unique_ptr<Common::Packet> packet)
    {
        if (!this->_isConnected)
        {
            Tools::error << "Network::SendPacket: Sending packet with no open socket.\n";
            return;
        }
        bool sendNext = false;
        {
            boost::unique_lock<boost::mutex> lock(this->_outMutex);
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

    std::list<Common::Packet*> Network::ProcessInPackets()
    {
        boost::unique_lock<boost::mutex> lock(this->_inMutex);
        return std::move(this->_inQueue);
    }

    void Network::_Disconnect()
    {
        try
        {
            this->_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
            this->_socket.close();
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
            boost::unique_lock<boost::mutex> lock(this->_outMutex);
            p = this->_outQueue.front();
        }
        std::vector<boost::asio::const_buffer> buffers;
        buffers.push_back(boost::asio::buffer(p->GetCompleteData(), p->GetCompleteSize()));
        boost::asio::async_write(this->_socket, buffers, boost::bind(&Network::_HandleWrite, this, boost::asio::placeholders::error));
    }

    void Network::_HandleWrite(boost::system::error_code const& error)
    {
        if (error)
        {
            Tools::error << "Network::_HandleWrite: Write error: \"" << error.message() << "\".\n";
            this->_Disconnect();
        }
        else
        {
            bool sendNext = false;
            {
                boost::unique_lock<boost::mutex> lock(this->_outMutex);
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

    void Network::_ReceivePacketSize()
    {
        boost::asio::async_read(this->_socket, boost::asio::buffer(this->_sizeBuffer), boost::bind(&Network::_HandleReceivePacketSize, this, boost::asio::placeholders::error));
    }

    void Network::_HandleReceivePacketSize(const boost::system::error_code& error)
    {
        if (error)
        {
            Tools::error << "Network::_HandleReceivePacketSize: Read error: \"" << error.message() << "\".\n";
            this->_Disconnect();
        }
        else
            this->_ReceivePacketContent(ntohs(*reinterpret_cast<Uint16*>(&this->_sizeBuffer[0])));
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
            this->_Disconnect();
        }
        else
        {
            auto p = new Common::Packet();
            p->SetData(this->_dataBuffer.data(), this->_dataBuffer.size());
            {
                boost::unique_lock<boost::mutex> lock(this->_inMutex);
                this->_inQueue.push_back(p);
            }
            this->_ReceivePacketSize();
        }
    }

}}
