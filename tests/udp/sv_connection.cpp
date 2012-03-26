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
        _udpSocket(socket->get_io_service()),
        _data(new Uint8[_bufferSize]),
        _size(_bufferSize),
        _offset(0),
        _toRead(0),
        _connected(true),
        _writeConnected(false),
        _udpWriteConnected(false),
        _udp(true)
    {
        _pt1.count = 0;
        _pt1.ok = false;
        try
        {
            //boost::asio::ip::udp::endpoint endpoint(socket->remote_endpoint().address(), 9999);
            boost::asio::ip::udp::endpoint endpoint(socket->remote_endpoint().address(), socket->remote_endpoint().port());
            this->_udpSocket.open(endpoint.protocol());
            this->_udpSocket.connect(endpoint);
            Tools::error << "UDP endpoint: " << socket->remote_endpoint().address() << ":" << socket->remote_endpoint().port() << ".\n";
        }
        catch (std::exception& e)
        {
            Tools::error << "Could not connect to UDP endpoint: " << e.what() << ".\n";
            this->_udp = false;
        }
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

    void Connection::SendUdpPacket(std::unique_ptr<Common::Packet>& packet)
    {
        std::function<void(void)> fx =
            std::bind(&Connection::_SendUdpPacket,
                    this->shared_from_this(),
                    Tools::Deleter<Common::Packet>::CreatePtr(packet.release()));
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

    void Connection::PassThrough1()
    {
        if (this->_connected == false)
        {
            std::cout << "already disconnected, bye\n";
            return;
        }
        if (this->_udp == false)
        {
            std::cout << "udp is false\n";
            try
            {
                this->_udpSocket.close();
                boost::asio::ip::udp::endpoint endpoint(_socket->remote_endpoint().address(), _socket->remote_endpoint().port());
                this->_udpSocket.open(endpoint.protocol());
                this->_udpSocket.connect(endpoint);
                Tools::error << "UDP endpoint: " << _socket->remote_endpoint().address() << ":" << _socket->remote_endpoint().port() << ".\n";
                this->_udp = true;
            }
            catch (std::exception& e)
            {
                Tools::error << "Could not connect to UDP endpoint: " << e.what() << ".\n";
                this->_udp = false;
                return;
            }
        }
        if (_pt1.count == 0)
            std::cout << "PassThrough 1, DEBUT\n";

        if (_pt1.ok == true)
        {
            std::cout << "pt1 ok\n";
            return;
        }

        std::cout << _pt1.count++ << "\n";

        if (_pt1.count == 200)
        {
            std::cout << "Toujours pas bon au bout de 20, passThrough suivant\n";
            return;
        }

        auto toto = PacketCreator::PassThrough(1);
        this->SendUdpPacket(toto);

        std::function<void(void)> fx =
            std::bind(&Connection::PassThrough1,
                    this->shared_from_this());
        this->_TimedDispatch(fx, 3000);
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

    void Connection::_SendUdpPacket(std::shared_ptr<Common::Packet> packet)
    {
        if (!this->_connected || !this->_socket)
        {
            Tools::debug << "Socket already down\n";
            return;
        }

        std::cout << "sendudppacket\n";

        //if (!this->_udp)
        //    this->_SendPacket(packet);

        this->_toSendUdpPackets.push(std::unique_ptr<Common::Packet>(Tools::Deleter<Common::Packet>::StealPtr(packet)));
        if (!this->_udpWriteConnected)
            this->_ConnectUdpWrite();
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

    void Connection::_ConnectUdpWrite()
    {
        if (!this->_connected || !this->_socket)
            return;

        if (this->_toSendUdpPackets.size() == 0 || this->_udpWriteConnected == true)
            return;
        this->_udpWriteConnected = true;
        std::unique_ptr<Common::Packet> packet(std::move(this->_toSendUdpPackets.front()));
        this->_toSendUdpPackets.pop();

        std::vector<boost::asio::const_buffer> buffers;
        buffers.push_back(boost::asio::buffer(packet->GetData(), packet->GetSize()));
        this->_udpSocket.async_send(
                buffers,
                boost::bind(
                    &Connection::_HandleUdpWrite,
                    this->shared_from_this(),
                    boost::shared_ptr<Common::Packet>(packet.release()),
                    boost::asio::placeholders::error)
                );
        std::cout << "connectudpwrite\n";

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

    void Connection::_HandleUdpWrite(boost::shared_ptr<Common::Packet> /*packetSent*/,
                                           boost::system::error_code const error)
    {
        this->_udpWriteConnected = false;
        if (!error)
        {
            std::cout << "_handleudpwrite ok\n";
            this->_ConnectUdpWrite();
        }
        else
        {
            std::cout << "_HANDLEUDPWRITE error\n";
            this->_udp = false;
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
                    this->PassThrough1();
                }
                break;
            case (tst_protocol::ActionType)tst_protocol::ClientToServer::clPassThrough:
                {
                    Uint32 ptType;
                    PacketExtractor::PassThrough(*packet, ptType);

                    auto toto = PacketCreator::PassThroughOk(ptType);
                    this->SendPacket(toto);
                }
                break;
            case (tst_protocol::ActionType)tst_protocol::ClientToServer::clPassThroughOk:
                {
                    Uint32 ptType;
                    PacketExtractor::PassThroughOk(*packet, ptType);

                    switch (ptType)
                    {
                    case 1:
                        std::cout << "PT1OK\n";
                        _pt1.ok = true;
                        _udp = true;
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
