
#include <iostream>

#include "tools/ToString.hpp"
#include "common/Packet.hpp"
#include "Network.hpp"

namespace Client {

Network::Network(PacketCallback packetCallback,
                 ErrorCallback errorCallback) :
    _onPacket(packetCallback),
    _onError(errorCallback),
    _sending(false)
{
}

void Network::Connect(std::string const& host, Uint16 port)
{
    this->_sizeBuffer.resize(2);
    this->_socket = new boost::asio::ip::tcp::socket(this->_ioService);
    boost::asio::ip::tcp::resolver resolver(this->_ioService);
    boost::asio::ip::tcp::resolver::query query(host, Tools::ToString(port));
    boost::asio::ip::tcp::resolver::iterator endpointIterator = resolver.resolve(query);
    boost::asio::ip::tcp::resolver::iterator end;
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpointIterator != end)
    {
        this->_socket->close();
        this->_socket->connect(*endpointIterator++, error);
    }
    if (error)
    {
        std::cerr << "Network::Network: Connection failed ("
                  << host << ":" << port << ")." << std::endl;
        throw std::runtime_error("connection failed");
    }
    this->_host = host;
}

Network::~Network()
{
    Tools::Delete(this->_socket);
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

void Network::SendPacket(Common::PacketPtr packet, SentCallback cb)
{
    bool sendNext = false;
    {
        boost::unique_lock<boost::mutex> lock(this->_mutex);
        if (this->_socket)
        {
            ToSend toSend(packet.release(), cb);
            this->_outQueue.push(toSend);
        }
        else
            std::cerr << "Network::SendPacket while disconnected." << std::endl;
        if (this->_socket && !this->_sending)
        {
            this->_sending = true;
            sendNext = true;
        }
    }
    if (sendNext)
        this->_SendNext();
}

void Network::_Disconnect(boost::system::error_code const& error)
{
    try
    {
        this->_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        this->_socket->close();
    }
    catch (std::exception& e)
    {
        std::cerr << "Network::_Disconnect: Socket shutdown: \"" << e.what() << "\"." << std::endl;
    }
    Tools::Delete(this->_socket);
    this->_socket = 0;
    std::cerr << "Network::_Disconnect: Socket disconnected." << std::endl;
    this->_onError(error.message());
}

void Network::_SendNext()
{
    Common::Packet* p;
    {
        boost::unique_lock<boost::mutex> lock(this->_mutex);
        p = std::get<0>(this->_outQueue.front());
    }
    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back(boost::asio::buffer(p->GetCompleteData(), p->GetCompleteSize()));
    boost::asio::async_write(*this->_socket, buffers, boost::bind(&Network::_HandleWrite, this, boost::asio::placeholders::error));
}

void Network::_HandleWrite(boost::system::error_code const& error)
{
    if (error)
    {
        std::cerr << "Network::_HandleWrite: Write error: \"" << error.message() << "\"." << std::endl;
        this->_Disconnect(error);
    }
    else
    {
        SentCallback cb;
        bool sendNext = false;
        {
            boost::unique_lock<boost::mutex> lock(this->_mutex);
            auto tuple = this->_outQueue.front();
            delete std::get<0>(tuple);
            cb = std::get<1>(tuple);
            this->_outQueue.pop();
            if (this->_outQueue.empty())
                this->_sending = false;
            else
                sendNext = true;
        }
        if (sendNext)
            this->_SendNext();
        if (cb)
        {
            try
            {
                cb();
            }
            catch (std::exception& err)
            {
                std::cerr << "Network::_HandleWrite(): packet sent callback failed: " << err.what() << ".\n";
            }
        }
    }
}

void Network::_ReceivePacketSize()
{
    boost::asio::async_read(*this->_socket, boost::asio::buffer(this->_sizeBuffer), boost::bind(&Network::_HandleReceivePacketSize, this, boost::asio::placeholders::error));
}

void Network::_HandleReceivePacketSize(const boost::system::error_code& error)
{
    if (error)
    {
        std::cerr << "Network::_HandleReceivePacketSize: Read error: \"" << error.message() << "\"." << std::endl;
        this->_Disconnect(error);
    }
    else
        this->_ReceivePacketContent(ntohs(*reinterpret_cast<Uint16*>(&this->_sizeBuffer[0])));
}

void Network::_ReceivePacketContent(unsigned int size)
{
    this->_dataBuffer.resize(size);
    boost::asio::async_read(*this->_socket, boost::asio::buffer(this->_dataBuffer), boost::bind(&Network::_HandleReceivePacketContent, this, boost::asio::placeholders::error));
}

void Network::_HandleReceivePacketContent(const boost::system::error_code& error)
{
    if (error)
    {
        std::cerr << "Network::_HandleReceivePacketContent: Read error \"" << error.message() << "\"." << std::endl;
        this->_Disconnect(error);
    }
    else
    {
        auto p = new Common::Packet;
        p->SetData(&this->_dataBuffer[0], this->_dataBuffer.size());
        Common::PacketPtr ptr(p);
        this->_onPacket(ptr);
        this->_ReceivePacketSize();
    }
}

}
