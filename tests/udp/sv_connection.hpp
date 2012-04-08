#ifndef __SV_CONNECTION_HPP__
#define __SV_CONNECTION_HPP__

#include "common/Packet.hpp"
#include "sv_udppacket.hpp"

namespace sv {

    class Network;

    class Connection :
        public boost::enable_shared_from_this<Connection>,
        private boost::noncopyable
    {
    private:
        static const unsigned int _bufferSize = 8192;

        Network& _network;
        boost::asio::io_service& _ioService;
        boost::asio::ip::tcp::socket* _socket;
        Uint8* _data;
        size_t _size;
        size_t _offset;
        size_t _toRead;
        std::queue<std::unique_ptr<Common::Packet>> _toSendPackets;
        std::queue<std::unique_ptr<UdpPacket>> _toSendUdpPackets;
        bool _connected;
        bool _writeConnected;

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
        Connection(Network& network, boost::asio::ip::tcp::socket* socket);
        ~Connection();

        // threadsafe
        void SendPacket(std::unique_ptr<Common::Packet>& packet);
        void SendUdpPacket(std::unique_ptr<UdpPacket>& packet);
        void Shutdown();
        void ConnectRead();
        void HandlePacket(std::unique_ptr<Tools::ByteArray>& packet, boost::asio::ip::udp::endpoint const& sender);

        std::unique_ptr<Common::Packet> GetUdpPacket();
        boost::asio::ip::udp::endpoint const& GetEndpoint() const { return this->_udpStatus.endpoint; }

        void PassThrough1();

    private:
        void _Shutdown();
        void _HandleError(boost::system::error_code const& error);
        void _SendPacket(std::shared_ptr<Common::Packet> packet);
        void _SendUdpPacket(std::shared_ptr<UdpPacket> packet);
        void _ConnectRead();
        void _ConnectWrite();
        void _HandleRead(boost::system::error_code const error,
                         std::size_t transferredBytes);
        void _HandleWrite(boost::shared_ptr<Common::Packet> packetSent,
                          boost::system::error_code const error,
                          std::size_t bytes_transferred);

        void _HandlePacket(std::unique_ptr<Tools::ByteArray>& packet);

        void _TimedDispatch(std::function<void(void)> fx, Uint32 ms);
        void _ExecDispatch(std::function<void(void)>& message,
                std::shared_ptr<boost::asio::deadline_timer>,
                boost::system::error_code const& error);
    };

}

#endif
