#include "server2/network/ClientConnection.hpp"

#include "server2/clientmanagement/ClientManager.hpp"

#include "common/Packet.hpp"

#ifdef _WIN32
// Desactive le warning "longueur du nom décoré dépassée, le nom a été tronqué"
# pragma warning(disable: 4503)
#endif

namespace Server { namespace Network {

    ClientConnection::ClientConnection(boost::asio::ip::tcp::socket* socket,
                                       ClientManagement::ClientManager& clientManager) :
        _clientManager(clientManager),
        _ioService(socket->get_io_service()),
        _socket(std::move(socket)),
        _data(new Uint8[_bufferSize]),
        _size(_bufferSize),
        _offset(0),
        _toRead(0),
        _connected(true),
        _writeConnected(false),
        _clientId(0)
    {
        assert(this->_socket);
    }

    ClientConnection::~ClientConnection()
    {
        this->Shutdown();
        Tools::DeleteTab(this->_data);
        Tools::Delete(this->_socket);
    }

    void ClientConnection::SetClientId(Uint32 id)
    {
        this->_clientId = id;
    }

    void ClientConnection::_Shutdown()
    {
        if (this->_socket)
        {
            Tools::debug << "Shutting down ClientConnection\n";
            Tools::Delete(this->_socket);
            this->_socket = 0;
            this->_connected = false;
            this->_this.reset();
            Tools::debug << "ClientConnection is Down\n";
        }
    }

    void ClientConnection::_HandleRead(boost::system::error_code const error,
                                       std::size_t transferredBytes)
    {
        std::list<std::unique_ptr<Common::Packet>> packets;
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
                        Common::Packet* packet = new Common::Packet();
                        packets.push_back(std::unique_ptr< Common::Packet >(packet));
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
            Tools::log << "ClientConnection error: " << error << ": " << error.message() << "(client " << this->_clientId << ")\n";
            if (this->_connected)
                this->_clientManager.HandleClientError(this->_clientId);
            this->_connected = false;
            return;
        }

        for (auto it = packets.begin(), ite = packets.end(); it != ite; ++it)
            this->_clientManager.HandlePacket(this->_clientId, it->release());
    }

    void ClientConnection::_ConnectRead()
    {
        if (!this->_this)
            this->_this = this->shared_from_this();
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

    void ClientConnection::_SendPacket(Common::Packet* packet_)
    {
        std::unique_ptr<Common::Packet> packet(packet_);
        if (!this->_connected || !this->_socket)
        {
            Tools::debug << "Socket already down (client " << this->_clientId << ")\n";
            return;
        }
        this->_toSendPackets.push(std::move(packet));
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
            Tools::log << "ClientConnection error: " << error << ": " << error.message() << "(client " << this->_clientId << ")\n";
            if (this->_connected)
                this->_clientManager.HandleClientError(this->_clientId);
            this->_connected = false;
        }
    }

    void ClientConnection::_ConnectWrite()
    {
        assert(this->_clientId != 0);
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
