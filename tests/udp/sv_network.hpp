#ifndef __SERVER_NETWORK_NETWORK_HPP__
#define __SERVER_NETWORK_NETWORK_HPP__

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
        size_t _size;
        Uint8* _data;
        Uint32 _nextId;

        std::map<Uint32, boost::shared_ptr<Connection>> _connections;

    public:
        Network();
        ~Network();

        void Run();
        void Stop();

        void RemoveConnection(boost::shared_ptr<Connection> conn);
    private:
        void _ConnectAccept();
        void _HandleAccept(boost::system::error_code const& e);
        void _ConnectUdpRead();
        void _HandleUdpRead(boost::system::error_code const& e, std::size_t size);

        void _HandleNewConnection(boost::shared_ptr<Connection> conn);
        void _HandleUdpPacket(std::unique_ptr<Tools::ByteArray>& packet);
    };

}

#endif
