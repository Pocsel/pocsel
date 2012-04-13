#ifndef __SERVER_NETWORK_CLIENTCONNECTION_HPP__
#define __SERVER_NETWORK_CLIENTCONNECTION_HPP__

#include "common/Packet.hpp"
#include "server/network/UdpPacket.hpp"

namespace Server {
    namespace Network {
        class Network;
    }
    namespace ClientManagement {
        class ClientManager;
    }
}

namespace Server { namespace Network {

    class ClientConnection :
        public boost::enable_shared_from_this<ClientConnection>,
        private boost::noncopyable
    {
    public:
        typedef std::function<void(Uint32)> ErrorCallback;
        typedef std::function<void(Uint32, std::unique_ptr<Tools::ByteArray>&)> PacketCallback;

    private:
        Network& _network;
        Uint32 _id;
        boost::asio::io_service& _ioService;
        boost::asio::ip::tcp::socket* _socket;
        std::vector<char> _packetSizeBuffer;
        std::vector<char> _packetContentBuffer;
        std::queue<std::unique_ptr<Common::Packet>> _toSendPackets;
        std::queue<std::unique_ptr<UdpPacket>> _toSendUdpPackets;
        bool _connected;
        bool _writeConnected;
        bool _udpWriteConnected;
        ErrorCallback _errorCallback;
        PacketCallback _packetCallback;

        struct {
            bool udpReadySent;
            bool udpReady;

            bool endpointKnown;
            boost::asio::ip::udp::endpoint endpoint;

            bool passThroughActive;
            unsigned int passThroughCount;

            bool ptOkSent;
        } _udpStatus;

    public:
        ClientConnection(
                Network& network,
                Uint32 id,
                boost::asio::ip::tcp::socket* socket,
                ErrorCallback& errorCallback,
                PacketCallback& packetCallback);
        ~ClientConnection();

        // threadsafe
        void SendPacket(std::unique_ptr<Common::Packet> packet);
        void SendUdpPacket(std::unique_ptr<UdpPacket> packet);
        void Shutdown();
        void ConnectRead();

        // from Network
        bool CheckEndpoint(boost::asio::ip::udp::endpoint const& sender);
        void HandlePacket(std::unique_ptr<Tools::ByteArray>& packet); // rcv a 'clean' packet
        std::unique_ptr<UdpPacket> GetUdpPacket();
        boost::asio::ip::udp::endpoint const& GetEndpoint() const { return this->_udpStatus.endpoint; }

    private:
        void _Shutdown();
        void _HandleError(boost::system::error_code const& error);
        void _SendPacket(std::shared_ptr<Common::Packet> packet);
        void _SendUdpPacket(std::shared_ptr<UdpPacket> packet);
        void _ConnectReadPacketSize();
        void _ConnectReadPacketContent(std::size_t size);
        void _ConnectWrite();
        void _HandleReadPacketSize(boost::system::error_code const& error);
        void _HandleReadPacketContent(boost::system::error_code const& error);
        void _HandleWrite(boost::shared_ptr<Common::Packet> packetSent,
                          boost::system::error_code const& error);

        void _PassThrough();
        void _TimedDispatch(std::function<void(void)> fx, Uint32 ms);
        void _ExecDispatch(std::function<void(void)>& message,
                std::shared_ptr<boost::asio::deadline_timer>,
                boost::system::error_code const& error);
    };

}}

#endif
