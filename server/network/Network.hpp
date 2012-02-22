#ifndef __SERVER_NETWORK_NETWORK_HPP__
#define __SERVER_NETWORK_NETWORK_HPP__

namespace Common {
    class Packet;
}

namespace Server {
    class Server;
    class Settings;
}

namespace Server { namespace Network {

    class ClientConnection;

    class Network :
        private boost::noncopyable
    {
    public:
        typedef std::function<void(boost::shared_ptr<ClientConnection>)> NewConnectionHandler;
        typedef std::function<void(std::unique_ptr<Common::Packet>&)> UdpPacketHandler;

    private:
        static const unsigned int _buffSize = 1024;
        Server& _server;
        NewConnectionHandler _newConnectionHandler;
        UdpPacketHandler _udpPacketHandler;
        boost::asio::io_service _ioService;
        boost::asio::ip::tcp::acceptor _acceptor;
        boost::asio::ip::tcp::socket* _newConnection;
        boost::asio::ip::udp::socket _udpSocket;
        size_t _size;
        Uint8* _data;

    public:
        Network(Server& server, NewConnectionHandler& newConnectionHandler, UdpPacketHandler& udpPacketHandler);
        ~Network();

        void Run();
        void Stop();
    private:
        void _ConnectAccept();
        void _HandleAccept(boost::system::error_code const& e);
        void _ConnectUdpRead();
        void _HandleUdpRead(boost::system::error_code const& e, std::size_t size);
    };

}}

#endif
