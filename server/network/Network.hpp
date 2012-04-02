#ifndef __SERVER_NETWORK_NETWORK_HPP__
#define __SERVER_NETWORK_NETWORK_HPP__

namespace Tools {
    class ByteArray;
}

namespace Server {
    class Server;
    class Settings;

    namespace Network {
        class ClientConnection;
        class UdpPacket;
    }
}

namespace Server { namespace Network {

    class Network :
        private boost::noncopyable
    {
    public:
        typedef std::function<void(Uint32, boost::shared_ptr<ClientConnection>)> NewConnectionHandler;
        typedef std::function<void(Uint32, std::unique_ptr<Tools::ByteArray>&)> PacketHandler;
        typedef std::function<void(Uint32)> ErrorHandler;

    private:
        static const unsigned int _buffSize = 4096;
        Server& _server;
        Uint32 _nextId;
        NewConnectionHandler _newConnectionHandler;
        PacketHandler _packetHandler;
        ErrorHandler _errorHandler;
        boost::asio::io_service _ioService;
        boost::asio::ip::tcp::acceptor _acceptor;
        boost::asio::ip::tcp::socket* _newConnection;
        boost::asio::ip::udp::socket _udpSocket;
        boost::asio::ip::udp::endpoint _udpSenderEndpoint;
        size_t _size;
        Uint8* _data;

        std::queue<Uint32> _sendingConnections;
        bool _udpWriteConnected;

        std::map<Uint32, boost::shared_ptr<ClientConnection>> _connections;

    public:
        Network(Server& server,
                NewConnectionHandler& newConnectionHandler,
                PacketHandler& packetHandler,
                ErrorHandler& errorHandler);
        ~Network();

        void Run();
        void Stop();
        void SendUdpPacket(Uint32 connId);
        void RemoveConnection(Uint32 connId);
    private:
        void _ConnectAccept();
        void _HandleAccept(boost::system::error_code const& e);
        void _ConnectUdpRead();
        void _HandleUdpRead(boost::system::error_code const& e, std::size_t size);
        void _ConnectUdpWrite();
        void _HandleUdpWrite(boost::shared_ptr<UdpPacket> packetSent,
                             boost::system::error_code const error);
        Uint32 _GetNextId();
    };

}}

#endif
