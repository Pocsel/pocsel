#ifndef __SERVER_NETWORK_Connection_HPP__
#define __SERVER_NETWORK_Connection_HPP__

#include "common/Packet.hpp"

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
        boost::asio::ip::udp::socket _udpSocket;
        Uint8* _data;
        size_t _size;
        size_t _offset;
        size_t _toRead;
        std::queue<std::unique_ptr<Common::Packet>> _toSendPackets;
        std::queue<std::unique_ptr<Common::Packet>> _toSendUdpPackets;
        bool _connected;
        bool _writeConnected;
        bool _udpWriteConnected;
        bool _udp;

    public:
        Connection(Network& network, boost::asio::ip::tcp::socket* socket);
        ~Connection();

        // threadsafe
        void SendPacket(std::unique_ptr<Common::Packet> packet);
        void SendUdpPacket(std::unique_ptr<Common::Packet> packet);
        void Shutdown();
        void ConnectRead();

    private:
        void _Shutdown();
        void _HandleError(boost::system::error_code const& error);
        void _SendPacket(std::shared_ptr<Common::Packet> packet);
        void _SendUdpPacket(std::shared_ptr<Common::Packet> packet);
        void _ConnectRead();
        void _ConnectWrite();
        void _ConnectUdpWrite();
        void _HandleRead(boost::system::error_code const error,
                         std::size_t transferredBytes);
        void _HandleWrite(boost::shared_ptr<Common::Packet> packetSent,
                          boost::system::error_code const error,
                          std::size_t bytes_transferred);
        void _HandleUdpWrite(boost::shared_ptr<Common::Packet> packetSent,
                          boost::system::error_code const error);

        void _HandlePacket(std::unique_ptr<Tools::ByteArray>& packet);
    };

}

#endif
