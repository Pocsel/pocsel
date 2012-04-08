#ifndef __SV_NETWORK_HPP__
#define __SV_NETWORK_HPP__

namespace Common {
    class Packet;
}

namespace Tools {
    class ByteArray;
}

namespace sv {

    class Connection;

    class Network :
        private boost::noncopyable
    {
    public:
        static const struct Settings
        {
            static std::string const host;
            static std::string const port;
        } settings;

    private:
        static const unsigned int _buffSize = 1024;
        boost::asio::io_service _ioService;
        boost::asio::ip::tcp::acceptor _acceptor;
        boost::asio::ip::tcp::socket* _newConnection;
        boost::asio::ip::udp::socket _udpSocket;
        boost::asio::ip::udp::endpoint _udpSenderEndpoint;
        size_t _size;
        Uint8* _data;
        Uint32 _nextId;

        std::queue<boost::shared_ptr<Connection>> _sendingConnections;
        bool _udpWriteConnected;

        std::map<Uint32, boost::shared_ptr<Connection>> _connections;

    public:
        Network();
        ~Network();

        void Run();
        void Stop();

        void IHasPacketToSend(boost::shared_ptr<Connection> conn);
        void RemoveConnection(boost::shared_ptr<Connection> conn);
    private:
        void _ConnectAccept();
        void _HandleAccept(boost::system::error_code const& e);
        void _ConnectUdpRead();
        void _HandleUdpRead(boost::system::error_code const& e, std::size_t size);
        void _ConnectUdpWrite();
        void _HandleUdpWrite(boost::shared_ptr<Common::Packet> packetSent,
                          boost::system::error_code const error);

        void _HandleNewConnection(boost::shared_ptr<Connection> conn);
        void _HandleUdpPacket(std::unique_ptr<Tools::ByteArray>& packet);
    };

}

#endif
