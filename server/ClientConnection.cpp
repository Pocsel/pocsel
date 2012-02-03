#include <iostream>
#include <algorithm>
#include <list>
#include <boost/bind.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/placeholders.hpp>

#include "server/ClientConnection.hpp"

#include "common/Packet.hpp"

#include "tools/ToString.hpp"
#include "tools/Delete.hpp"

#include "protocol/protocol.hpp"


#ifdef _WIN32
// Desactive le warning "longueur du nom décoré dépassée, le nom a été tronqué"
# pragma warning(disable: 4503)
#endif

namespace Server {

    ClientConnection::ClientConnection(SocketPtr& socket, HandlePacketCallback onPacket, HandleErrorCallback onError) :
        _ioService(socket->get_io_service()),
        _socket(std::move(socket)),
        _data(new Uint8[4096]),
        _size(4096),
        _offset(0),
        _toRead(0),
        _onPacket(onPacket),
        _onError(onError),
        _writeConnected(false)
    {
        assert(this->_socket);
        assert(this->_onPacket != 0);
        assert(this->_onError != 0);
    }

    ClientConnection::~ClientConnection()
    {
        std::cout << "ClientConnection::~ClientConnection()" <<std::endl;
        this->Shutdown();
        Tools::DeleteTab(this->_data);
    }

    void ClientConnection::_Shutdown()
    {
        std::cout << "Shutting down ClientConnection" << std::endl;
        if (this->_socket)
            this->_socket.reset();
        this->_onError = 0;
        this->_onPacket = 0;
        std::cout << "ClientConnection is Down" << std::endl;
    }

    void ClientConnection::_HandleRead(boost::system::error_code const error,
                                       std::size_t transferredBytes)
    {
        std::list<Common::PacketPtr> packets;
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
                        std::cout << "About to read " << this->_toRead << " bytes" << std::endl;
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
                        std::cerr << "Null size packet" << std::endl;
                    transferredBytes -= 2;
                    this->_offset += 2;
                }
                else
                {
                    if (transferredBytes >= this->_toRead)
                    {
                        std::cout << "Finalizing packet with " << this->_toRead << " bytes" << std::endl;
                        Common::Packet* packet = new Common::Packet();
                        packets.push_back(std::unique_ptr< Common::Packet >(packet));
                        this->_offset += this->_toRead;
                        transferredBytes -= this->_toRead;
                        this->_toRead = 0;
                        packet->SetData((char*)(this->_data + 2), this->_offset - 2);
                        if (transferredBytes > 0)
                            std::memmove(this->_data, this->_data + this->_offset, transferredBytes);
                        this->_offset = 0;
                    }
                    else
                    {
                        std::cout << "Append to packet " << transferredBytes << " bytes" << std::endl;
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
            std::cerr << "ClientConnection _HandleRead error: " << error << ": " << error.message() << std::endl;
            if (this->_onError != 0)
                this->_onError(error);
            this->_onPacket = 0;
            this->_onError = 0;
            return;
        }
        if (this->_onPacket != 0)
        {
            std::cout << "Processing packets" << std::endl;
            std::for_each(packets.begin(), packets.end(), this->_onPacket);
            std::cout << "Processing packets done" << std::endl;
        }
        else
            std::cout << "No packet handler defined !" << std::endl;
    }

    void ClientConnection::_ConnectRead()
    {
        if (!this->_onError || !this->_onPacket || !this->_socket)
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
        Common::PacketPtr packet(packet_);
        if (!packet)
        {
            std::cerr << "Null packet to send !" << std::endl;
            return;
        }
        else if (!this->_socket || !this->_onPacket || !this->_onError)
        {
            std::cerr << "Socket already down" << std::endl;
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
            std::cerr << "ClientConnection _HandleWrite error: " << error << ": " << error.message() << std::endl;
            if (this->_onError)
                this->_onError(error);
            this->_onError = 0;
            this->_onPacket = 0;
        }
    }

    void ClientConnection::_ConnectWrite()
    {
        if (!this->_onError || !this->_onPacket || !this->_socket)
            return;

        if (this->_toSendPackets.size() == 0 || this->_writeConnected == true)
            return;
        this->_writeConnected = true;
        Common::PacketPtr packet(std::move(this->_toSendPackets.front()));
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

}
