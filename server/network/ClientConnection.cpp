#include "server/network/ClientConnection.hpp"
#include "server/network/Network.hpp"
#include "server/network/PacketCreator.hpp"
#include "server/network/PacketExtractor.hpp"
#include "server/network/UdpPacket.hpp"

#include "common/Packet.hpp"

#include "tools/Deleter.hpp"

#include "protocol/protocol.hpp"

#ifdef _WIN32
// Desactive le warning "longueur du nom décoré dépassée, le nom a été tronqué"
# pragma warning(disable: 4503)
#endif

namespace Server { namespace Network {

    ClientConnection::ClientConnection(
            Network& network,
            Uint32 id,
            boost::asio::ip::tcp::socket* socket,
            ErrorCallback& errorCallback,
            PacketCallback& packetCallback) :
        _network(network),
        _id(id),
        _ioService(socket->get_io_service()),
        _socket(socket),
        _connected(true),
        _writeConnected(false),
        _errorCallback(errorCallback),
        _packetCallback(packetCallback)
    {
        _packetSizeBuffer.resize(Common::Packet::SizeBytes);
        _udpStatus.udpReadySent = false;
        _udpStatus.udpReady = false;
        _udpStatus.endpointKnown = false;
        _udpStatus.passThroughActive = false;
        _udpStatus.passThroughCount = 0;
        _udpStatus.ptOkSent = false;
    }

    ClientConnection::~ClientConnection()
    {
        Tools::Delete(this->_socket);
    }

    void ClientConnection::SendPacket(std::unique_ptr<Common::Packet> packet)
    {
        std::function<void(void)> fx =
            std::bind(&ClientConnection::_SendPacket,
                    this->shared_from_this(),
                    Tools::Deleter<Common::Packet>::CreatePtr(packet.release()));
        this->_ioService.dispatch(fx);
    }

    void ClientConnection::SendUdpPacket(std::unique_ptr<UdpPacket> packet)
    {
        std::function<void(void)> fx =
            std::bind(&ClientConnection::_SendUdpPacket,
                    this->shared_from_this(),
                    Tools::Deleter<UdpPacket>::CreatePtr(packet.release()));
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
            std::bind(&ClientConnection::_ConnectReadPacketSize, this->shared_from_this());
        this->_ioService.dispatch(fx);
    }

    bool ClientConnection::CheckEndpoint(boost::asio::ip::udp::endpoint const& sender)
    {
        if (this->_udpStatus.endpointKnown == false)
        {
            this->_udpStatus.endpoint = sender;
            this->_udpStatus.endpointKnown = true;
        }
        if (sender != this->_udpStatus.endpoint)
            return false;
        return true;
    }

    std::unique_ptr<UdpPacket> ClientConnection::GetUdpPacket()
    {
        std::unique_ptr<UdpPacket> ret = std::move(this->_toSendUdpPackets.front());
        this->_toSendUdpPackets.pop();
        return ret;
    }

    void ClientConnection::HandlePacket(std::unique_ptr<Tools::ByteArray>& packet)
    {
        if (packet->GetBytesLeft() < sizeof(Protocol::ActionType))
        {
            Tools::error << "Received an empty packet\n";
            return ;
        }
        char const* data = packet->GetData();

        static_assert(sizeof(Protocol::ActionType) == sizeof(Uint8), "actiontype doit etre un char ici");
        Protocol::ActionType action = data[0];

        if (action != (Protocol::ActionType)Protocol::ClientToServer::UdpReady &&
            action != (Protocol::ActionType)Protocol::ClientToServer::ClPassThrough &&
            action != (Protocol::ActionType)Protocol::ClientToServer::ClPassThroughOk
            )
        {
            // ce packet est pas network specific
            this->_packetCallback(this->_id, packet);
            return;
        }

        try
        {
            Protocol::ActionType type;
            packet->Read(type);
            switch (type)
            {
            case (Protocol::ActionType)Protocol::ClientToServer::UdpReady:
                {
                    bool ready;
                    PacketExtractor::UdpReady(*packet, ready);

                    this->_udpStatus.udpReadySent = true;
                    this->_udpStatus.udpReady = ready;
                    if (ready == true &&
                        this->_udpStatus.endpointKnown == true &&
                        this->_udpStatus.passThroughCount == 0 &&
                        this->_udpStatus.passThroughActive == false)
                        this->_PassThrough();
                }
                break;
            case (Protocol::ActionType)Protocol::ClientToServer::ClPassThrough:
                {
                    if (this->_udpStatus.ptOkSent == false)
                    {
                        auto toto = PacketCreator::PassThroughOk();
                        this->_SendPacket(Tools::Deleter<Common::Packet>::CreatePtr(toto.release()));
                        this->_udpStatus.ptOkSent = true;
                    }
                    if (this->_udpStatus.udpReady == true &&
                        this->_udpStatus.endpointKnown == true &&
                        this->_udpStatus.passThroughCount == 0 &&
                        this->_udpStatus.passThroughActive == false)
                        this->_PassThrough();
                }
                break;
            case (Protocol::ActionType)Protocol::ClientToServer::ClPassThroughOk:
                {
                    this->_udpStatus.passThroughActive = true;
                }
                break;
            default:
                throw std::runtime_error("Unknown packet type (" + Tools::ToString(type) + ")");
            }
        }
        catch (std::exception& e)
        {
            Tools::error << "could not read network packet, client de merde: " << e.what() << "\n";
            this->_Shutdown();
            this->_errorCallback(this->_id);
        }
    }
    void ClientConnection::_Shutdown()
    {
        if (this->_socket)
        {
            Tools::debug << "Shutting down ClientConnection\n";
            this->_socket->close();
            this->_connected = false;
            this->_network.RemoveConnection(this->_id);
            Tools::debug << "ClientConnection is Down\n";
        }
    }

    void ClientConnection::_HandleError(boost::system::error_code const& error)
    {
        Tools::log << "ClientConnection error: " << error << ": " << error.message() << "\n";
        if (this->_connected)
        {
            this->_Shutdown();
            this->_errorCallback(this->_id);
        }
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

    void ClientConnection::_SendUdpPacket(std::shared_ptr<UdpPacket> packet)
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
            if (packet->forceUdp == true) // client cant receive shit!
                return;
            this->_SendPacket(Tools::Deleter<Common::Packet>::CreatePtr(Tools::Deleter<UdpPacket>::StealPtr(packet)));
            return;
        }

        this->_toSendUdpPackets.push(std::unique_ptr<UdpPacket>(Tools::Deleter<UdpPacket>::StealPtr(packet)));
        this->_network.SendUdpPacket(this->_id);
    }

    void ClientConnection::_ConnectReadPacketSize()
    {
        if (!this->_connected || !this->_socket)
            return;
        boost::asio::async_read(
            *this->_socket,
            boost::asio::buffer(this->_packetSizeBuffer),
            boost::bind(
                &ClientConnection::_HandleReadPacketSize,
                this->shared_from_this(),
                boost::asio::placeholders::error
                )
            );
    }

    void ClientConnection::_ConnectReadPacketContent(std::size_t size)
    {
        if (!this->_connected || !this->_socket)
            return;
        this->_packetContentBuffer.resize(size);
        boost::asio::async_read(
                *this->_socket,
                boost::asio::buffer(this->_packetContentBuffer),
                boost::bind(
                    &ClientConnection::_HandleReadPacketContent,
                    this->shared_from_this(),
                    boost::asio::placeholders::error
                    )
                );
    }

    void ClientConnection::_ConnectWrite()
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
                &ClientConnection::_HandleWrite, this->shared_from_this(),
                boost::shared_ptr<Common::Packet>(packet.release()),
                boost::asio::placeholders::error
            )
        );
    }

    void ClientConnection::_HandleReadPacketSize(const boost::system::error_code& error)
    {
        if (error)
        {
            this->_HandleError(error);
            return;
        }
        static_assert(Common::Packet::SizeBytes == 2, "il faut changer ce code si on change la taille des packets");
        this->_ConnectReadPacketContent(ntohs(*reinterpret_cast<Uint16*>(this->_packetSizeBuffer.data())));
    }

    void ClientConnection::_HandleReadPacketContent(const boost::system::error_code& error)
    {
        if (error)
        {
            this->_HandleError(error);
            return;
        }
        else
        {
            std::unique_ptr<Tools::ByteArray> p(new Tools::ByteArray());
            p->SetData(this->_packetContentBuffer.data(), (Uint32)this->_packetContentBuffer.size());
            this->HandlePacket(p);
            this->_ConnectReadPacketSize();
        }
    }

    void ClientConnection::_HandleWrite(boost::shared_ptr<Common::Packet> /*packetSent*/,
                                        boost::system::error_code const& error)
    {
        this->_writeConnected = false;
        if (!error)
            this->_ConnectWrite();
        else
            this->_HandleError(error);
    }

    void ClientConnection::_PassThrough()
    {
        if (this->_connected == false)
            return;
        if (this->_udpStatus.udpReady == false)
        {
            Tools::error << "Client can't receive UDP, no passthrough tentative\n";
            return;
        }

        std::function<void(void)> fx = std::bind(&ClientConnection::_PassThrough, this->shared_from_this());
        if (_udpStatus.passThroughActive == false)
        {
            if (_udpStatus.passThroughCount == 100)
            {
                Tools::error << "Too many UDP passthrough tentatives, connection is gonna be full TCP.\n";
                return;
            }
            this->_TimedDispatch(fx, 55);
        }
        else
        {
            this->_TimedDispatch(fx, 5555);
        }

        auto toto = PacketCreator::PassThrough();
        this->_SendUdpPacket(Tools::Deleter<UdpPacket>::CreatePtr(toto.release()));
    }

    void ClientConnection::_TimedDispatch(std::function<void(void)> fx, Uint32 ms)
    {
        boost::asio::deadline_timer* t = new boost::asio::deadline_timer(this->_ioService, boost::posix_time::milliseconds(ms));
        std::function<void(boost::system::error_code const& e)>
            function(std::bind(&ClientConnection::_ExecDispatch,
                               this->shared_from_this(),
                               fx,
                               std::shared_ptr<boost::asio::deadline_timer>(t),
                               std::placeholders::_1));
        t->async_wait(function);
    }

    void ClientConnection::_ExecDispatch(std::function<void(void)>& fx,
            std::shared_ptr<boost::asio::deadline_timer>,
            boost::system::error_code const& error)
    {
        if (error == boost::asio::error::operation_aborted)
        {
            Tools::debug << "Timer aborted\n";
            return;
        }
        fx();
    }

}}
