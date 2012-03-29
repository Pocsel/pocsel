#include "sv_connection.hpp"
#include "sv_network.hpp"
#include "sv_packetcreator.hpp"
#include "sv_packetextractor.hpp"

#include "tools/Deleter.hpp"

namespace sv {

    Connection::Connection(Network& network, boost::asio::ip::tcp::socket* socket) :
        _network(network),
        _ioService(socket->get_io_service()),
        _socket(socket),
        _data(new Uint8[_bufferSize]),
        _size(_bufferSize),
        _offset(0),
        _toRead(0),
        _connected(true),
        _writeConnected(false)
    {
        _udpStatus.udpReadySent = false;
        _udpStatus.udpReady = false;
        _udpStatus.endpointKnown = false;
        _udpStatus.passThroughActive = false;
        _udpStatus.passThroughCount = 0;
        _udpStatus.ptOkSent = false;
    }

    Connection::~Connection()
    {
        Tools::DeleteTab(this->_data);
        Tools::Delete(this->_socket);
    }

    void Connection::SendPacket(std::unique_ptr<Common::Packet>& packet)
    {
        std::function<void(void)> fx =
            std::bind(&Connection::_SendPacket,
                    this->shared_from_this(),
                    Tools::Deleter<Common::Packet>::CreatePtr(packet.release()));
        this->_ioService.dispatch(fx);
    }

    void Connection::SendUdpPacket(std::unique_ptr<UdpPacket>& packet)
    {
        std::function<void(void)> fx =
            std::bind(&Connection::_SendUdpPacket,
                    this->shared_from_this(),
                    Tools::Deleter<UdpPacket>::CreatePtr(packet.release()));
        this->_ioService.dispatch(fx);
    }

    void Connection::Shutdown()
    {
        std::function<void(void)> fx =
            std::bind(&Connection::_Shutdown, this->shared_from_this());
        this->_ioService.dispatch(fx);
    }

    void Connection::ConnectRead()
    {
        std::function<void(void)> fx =
            std::bind(&Connection::_ConnectRead, this->shared_from_this());
        this->_ioService.dispatch(fx);
    }

    void Connection::HandlePacket(std::unique_ptr<Tools::ByteArray>& packet, boost::asio::ip::udp::endpoint const& sender)
    {
        if (this->_udpStatus.endpointKnown == false)
        {
            this->_udpStatus.endpoint = sender;
            this->_udpStatus.endpointKnown = true;
        }
        else if (sender != this->_udpStatus.endpoint)
        {
            // TODO smthng
            // le sender est différent d'avant, c'est la cacastrophe.
        }

        this->_HandlePacket(packet);
    }

    std::unique_ptr<Common::Packet> Connection::GetUdpPacket()
    {
        std::unique_ptr<Common::Packet> ret = std::move(this->_toSendUdpPackets.front());
        this->_toSendUdpPackets.pop();
        return ret;
    }

    void Connection::PassThrough1()
    {
        std::cout << "PassThrough1()\n";
        if (this->_connected == false)
        {
            std::cout << "already disconnected, bye\n";
            return;
        }
        if (this->_udpStatus.udpReady == false)
        {
            std::cout << "client cant receive udp, bye\n";
            return;
        }

        std::function<void(void)> fx =
            std::bind(&Connection::PassThrough1,
                    this->shared_from_this());

        if (_udpStatus.passThroughActive == false)
        {
            std::cout << "count=" << _udpStatus.passThroughCount++ << "\n";
            if (_udpStatus.passThroughCount == 50)
            {
                std::cout << "trop d'essai, on laisse tomber l'udp\n";
                return;
            }
            this->_TimedDispatch(fx, 55);
        }
        else
        {
            this->_TimedDispatch(fx, 5555);
        }

        auto toto = PacketCreator::PassThrough(1);
        this->SendUdpPacket(toto);
    }

    void Connection::_Shutdown()
    {
        if (this->_socket)
        {
            Tools::debug << "Shutting down Connection\n";
            this->_socket->close();
            this->_connected = false;
            Tools::debug << "Connection is Down\n";
        }
    }

    void Connection::_HandleError(boost::system::error_code const& error)
    {
        Tools::log << "Connection error: " << error << ": " << error.message() << "\n";
        if (this->_connected)
        {
            this->_Shutdown();
            this->_network.RemoveConnection(this->shared_from_this());
        }
    }

    void Connection::_SendPacket(std::shared_ptr<Common::Packet> packet)
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

    void Connection::_SendUdpPacket(std::shared_ptr<UdpPacket> packet)
    {
        if (!this->_connected || !this->_socket)
        {
            Tools::debug << "Socket already down\n";
            return;
        }

        if (this->_udpStatus.udpReady == false ||
            this->_udpStatus.endpointKnown == false ||
            (packet->forceUdp == false && this->_udpStatus.passThroughActive == false))
        {
            if (packet->forceUdp == true)
            {
                std::cout << "cant send this shit\n";
                return;
            }
            this->_SendPacket(packet);
            return;
        }

        this->_toSendUdpPackets.push(std::unique_ptr<UdpPacket>(Tools::Deleter<UdpPacket>::StealPtr(packet)));
        this->_network.IHasPacketToSend(this->shared_from_this());
    }

    void Connection::_ConnectRead()
    {
        std::cout << "_ConnectRead() 0\n";
        if (!this->_connected || !this->_socket)
            return;
        std::cout << "_ConnectRead() 1\n";
        boost::asio::async_read(
            *this->_socket,
            boost::asio::buffer(this->_data + this->_offset, this->_size - this->_offset),
            boost::asio::transfer_at_least(this->_toRead == 0 ? 2 : this->_toRead),
            boost::bind(
                &Connection::_HandleRead, this->shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
            )
        );
    }

    void Connection::_ConnectWrite()
    {
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
                &Connection::_HandleWrite, this->shared_from_this(),
                boost::shared_ptr<Common::Packet>(packet.release()),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
            )
        );
    }

    void Connection::_HandleRead(boost::system::error_code const error,
                                       std::size_t transferredBytes)
    {
        std::cout << "_HandleRead()\n";
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
            std::cout << "_HANDLEREAD error\n";
            this->_HandleError(error);
            return;
        }

        for (auto it = packets.begin(), ite = packets.end(); it != ite; ++it)
            this->_HandlePacket(*it);
    }

    void Connection::_HandleWrite(boost::shared_ptr<Common::Packet> /*packetSent*/,
                                        boost::system::error_code const error,
                                        std::size_t /*bytes_transferred*/)
    {
        this->_writeConnected = false;
        if (!error)
            this->_ConnectWrite();
        else
        {
            std::cout << "_HANDLEWRITE error\n";
            this->_HandleError(error);
        }
    }

    void Connection::_HandlePacket(std::unique_ptr<Tools::ByteArray>& packet)
    {
        try
        {
            tst_protocol::ActionType type;
            packet->Read(type);

            switch (type)
            {
            case (tst_protocol::ActionType)tst_protocol::ClientToServer::clUdpReady:
                {
                    bool ready;
                    PacketExtractor::UdpReady(*packet, ready);

                    this->_udpStatus.udpReadySent = true;
                    this->_udpStatus.udpReady = ready;
                    if (ready == true &&
                        this->_udpStatus.endpointKnown == true &&
                        this->_udpStatus.passThroughCount == 0 &&
                        this->_udpStatus.passThroughActive == false)
                        this->PassThrough1();
                }
                break;
            case (tst_protocol::ActionType)tst_protocol::ClientToServer::clPassThrough:
                {
                    Uint32 ptType;
                    PacketExtractor::PassThrough(*packet, ptType);

                    if (this->_udpStatus.ptOkSent == false)
                    {
                        auto toto = PacketCreator::PassThroughOk(ptType);
                        this->SendPacket(toto);
                        this->_udpStatus.ptOkSent = true;
                    }
                    if (this->_udpStatus.udpReady == true &&
                        this->_udpStatus.endpointKnown == true &&
                        this->_udpStatus.passThroughCount == 0 &&
                        this->_udpStatus.passThroughActive == false)
                        this->PassThrough1();
                }
                break;
            case (tst_protocol::ActionType)tst_protocol::ClientToServer::clPassThroughOk:
                {
                    Uint32 ptType;
                    PacketExtractor::PassThroughOk(*packet, ptType);

                    switch (ptType)
                    {
                    case 1:
                        this->_udpStatus.passThroughActive = true;
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
            std::cout << "could not read packet, client de merde: " << e.what() << "\n";
        }
    }

    void Connection::_TimedDispatch(std::function<void(void)> fx, Uint32 ms)
    {
        boost::asio::deadline_timer* t = new boost::asio::deadline_timer(this->_ioService, boost::posix_time::milliseconds(ms));
        std::function<void(boost::system::error_code const& e)>
            function(std::bind(&Connection::_ExecDispatch,
                               this->shared_from_this(),
                               fx,
                               std::shared_ptr<boost::asio::deadline_timer>(t),
                               std::placeholders::_1));
        t->async_wait(function);
    }

    void Connection::_ExecDispatch(std::function<void(void)>& message,
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
