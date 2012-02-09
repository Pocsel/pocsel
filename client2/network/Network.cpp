#include <iostream>

#include "client2/network/Network.hpp"
#include "client2/Client.hpp"
#include "tools/ToString.hpp"
#include "common/Packet.hpp"

namespace Client { namespace Network {

    Network::Network(Client& client) :
        _socket(_ioService), _sending(false), _isConnected(false)
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
            std::cerr << "Network::Network: Connection to " << host << ":" << port << " failed." << std::endl;
            throw std::runtime_error("connection failed");
        }
        this->_host = host;
        this->_port = port;
        this->_isConnected = true;
    }

    Network::~Network()
    {
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
        this->_ioService.stop();
    }

    void Network::SendPacket(std::unique_ptr<Common::Packet> packet)
    {
        if (!this->_isConnected)
        {
            std::cerr << "Network::SendPacket: Sending packet with no open socket." << std::endl;
            return;
        }
        bool sendNext = false;
        {
            boost::unique_lock<boost::mutex> lock(this->_mutex);
            this->_outQueue.push(packet.release());
            if (!this->_sending)
            {
                this->_sending = true;
                sendNext = true;
            }
        }
        if (sendNext)
            this->_SendNext();
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
            std::cerr << "Network::_Disconnect: Exception on socket shutdown: \"" << e.what() << "\"." << std::endl;
            return;
        }
        std::cout << "Network::_Disconnect: Socket disconnected." << std::endl;
    }

    void Network::_SendNext()
    {
        Common::Packet* p;
        {
            boost::unique_lock<boost::mutex> lock(this->_mutex);
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
            std::cerr << "Network::_HandleWrite: Write error: \"" << error.message() << "\"." << std::endl;
            this->_Disconnect();
        }
        else
        {
            bool sendNext = false;
            {
                boost::unique_lock<boost::mutex> lock(this->_mutex);
                Tools::Delete(this->_outQueue.front());
                this->_outQueue.pop();
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
            std::cerr << "Network::_HandleReceivePacketSize: Read error: \"" << error.message() << "\"." << std::endl;
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
            std::cerr << "Network::_HandleReceivePacketContent: Read error \"" << error.message() << "\"." << std::endl;
            this->_Disconnect();
        }
        else
        {
            auto p = new Common::Packet;
            p->SetData(&this->_dataBuffer[0], this->_dataBuffer.size());
            // XXX process packet
            this->_ReceivePacketSize();
        }
    }

}}
