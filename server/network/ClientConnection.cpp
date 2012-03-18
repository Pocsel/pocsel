#include "server/network/ClientConnection.hpp"

#include "tools/Deleter.hpp"

#ifdef _WIN32
// Desactive le warning "longueur du nom décoré dépassée, le nom a été tronqué"
# pragma warning(disable: 4503)
#endif

namespace Server { namespace Network {

    ClientConnection::ClientConnection(boost::asio::ip::tcp::socket* socket) :
        _ioService(socket->get_io_service()),
        _socket(socket),
        _udpSocket(socket->get_io_service()),
        _data(new Uint8[_bufferSize]),
        _size(_bufferSize),
        _offset(0),
        _toRead(0),
        _connected(true),
        _writeConnected(false),
        _errorCallback(0),
        _packetCallback(0),
        _udp(true)
    {
        try
        {
            boost::asio::ip::udp::endpoint endpoint(socket->remote_endpoint().address(), socket->remote_endpoint().port());
            this->_udpSocket.open(endpoint.protocol());
            this->_udpSocket.connect(endpoint);
        }
        catch (std::exception& e)
        {
            Tools::error << "Could not connect to UDP endpoint: " << e.what() << ".\n";
            this->_udp = false;
        }
    }

    ClientConnection::~ClientConnection()
    {
        Tools::DeleteTab(this->_data);
        Tools::Delete(this->_socket);
    }

    void ClientConnection::SetCallbacks(ErrorCallback& errorCallback, PacketCallback& packetCallback)
    {
        this->_errorCallback = errorCallback;
        this->_packetCallback = packetCallback;
    }

    void ClientConnection::SendPacket(std::unique_ptr<Common::Packet> packet)
    {
        std::function<void(void)> fx =
            std::bind(&ClientConnection::_SendPacket,
                    this->shared_from_this(),
                    Tools::Deleter<Common::Packet>::CreatePtr(packet.release()));
        this->_ioService.dispatch(fx);
    }

    void ClientConnection::Shutdown()
    {
        std::function<void(void)> fx =
            std::bind(&ClientConnection::_Shutdown, this->shared_from_this());
        this->_ioService.dispatch(fx);
    }

    void ClientConnection::ConnectRead()
    {
        std::function<void(void)> fx =
            std::bind(&ClientConnection::_ConnectRead, this->shared_from_this());
        this->_ioService.dispatch(fx);
    }

    void ClientConnection::_Shutdown()
    {
        if (this->_socket)
        {
            Tools::debug << "Shutting down ClientConnection\n";
            this->_socket->close();
            this->_connected = false;
            Tools::debug << "ClientConnection is Down\n";
        }
    }

    void ClientConnection::_HandleError(boost::system::error_code const& error)
    {
        Tools::log << "ClientConnection error: " << error << ": " << error.message() << "\n";
        if (this->_connected)
        {
            this->_Shutdown();
            this->_errorCallback();
        }
    }

    void ClientConnection::_HandleRead(boost::system::error_code const error,
                                       std::size_t transferredBytes)
    {
        std::list<std::unique_ptr<Tools::ByteArray>> packets;
        if (!error)
        {
            assert(transferredBytes >= 2);
            while (transferredBytes > 0)
            {
                if (this->_toRead == 0)
                {
                    this->_toRead = Common::ArrayToUint16(this->_data + this->_offset);
                    if (this->_toRead > 0)
                    {
                        if (this->_toRead > this->_size)
                        {
                            this->_size += this->_toRead * 2;
                            Uint8* data = new Uint8[this->_size];
                            std::memcpy(data, this->_data, transferredBytes);
                            Tools::DeleteTab(this->_data);
                            this->_data = data;
                        }
                    }
                    else
                        Tools::log << "Null sized packet received.\n";
                    transferredBytes -= 2;
                    this->_offset += 2;
                }
                else
                {
                    if (transferredBytes >= this->_toRead)
                    {
                        Tools::ByteArray* packet = new Tools::ByteArray();
                        packets.push_back(std::unique_ptr<Tools::ByteArray>(packet));
                        this->_offset += this->_toRead;
                        transferredBytes -= this->_toRead;
                        this->_toRead = 0;
                        packet->SetData((char*)(this->_data + 2), static_cast<Uint16>(this->_offset - 2));
                        if (transferredBytes > 0)
                            std::memmove(this->_data, this->_data + this->_offset, transferredBytes);
                        this->_offset = 0;
                    }
                    else
                    {
                        this->_offset += transferredBytes;
                        this->_toRead -= transferredBytes;
                        transferredBytes = 0;
                        break;
                    }
                }
            }
            this->_ConnectRead();
        }
        else
        {
            this->_HandleError(error);
            return;
        }

        for (auto it = packets.begin(), ite = packets.end(); it != ite; ++it)
            this->_packetCallback(*it);
    }

    void ClientConnection::_ConnectRead()
    {
        if (!this->_connected || !this->_socket)
            return;
        boost::asio::async_read(
            *this->_socket,
            boost::asio::buffer(this->_data + this->_offset, this->_size - this->_offset),
            boost::asio::transfer_at_least(this->_toRead == 0 ? 2 : this->_toRead),
            boost::bind(
                &ClientConnection::_HandleRead, this->shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
            )
        );
    }

    void ClientConnection::_SendPacket(std::shared_ptr<Common::Packet> packet)
    {
        if (!this->_connected || !this->_socket)
        {
            Tools::debug << "Socket already down\n";
            return;
        }
        this->_toSendPackets.push(std::unique_ptr<Common::Packet>(Tools::Deleter<Common::Packet>::StealPtr(packet)));
        if (!this->_writeConnected)
            this->_ConnectWrite();
    }

    void ClientConnection::_HandleWrite(boost::shared_ptr<Common::Packet> /*packetSent*/,
                                        boost::system::error_code const error,
                                        std::size_t /*bytes_transferred*/)
    {
        this->_writeConnected = false;
        if (!error)
        {
            this->_ConnectWrite();
        }
        else
        {
            this->_HandleError(error);
        }
    }

    void ClientConnection::_ConnectWrite()
    {
        assert(this->_errorCallback != 0 && "need une ErrorCallback");
        assert(this->_packetCallback != 0 && "need une PacketCallback");
        if (!this->_connected || !this->_socket)
            return;

        if (this->_toSendPackets.size() == 0 || this->_writeConnected == true)
            return;
        this->_writeConnected = true;
        std::unique_ptr<Common::Packet> packet(std::move(this->_toSendPackets.front()));
        this->_toSendPackets.pop();

        std::vector<boost::asio::const_buffer> buffers;
        buffers.push_back(boost::asio::buffer(packet->GetCompleteData(), packet->GetCompleteSize()));
        boost::asio::async_write(*this->_socket, buffers,
            boost::bind(
                &ClientConnection::_HandleWrite, this->shared_from_this(),
                boost::shared_ptr<Common::Packet>(packet.release()),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
            )
        );
    }

}}
