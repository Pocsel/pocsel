#ifndef __SERVER_NETWORK_CLIENTCONNECTION_HPP__
#define __SERVER_NETWORK_CLIENTCONNECTION_HPP__

namespace Common {

    class Packet;

}

namespace Server { namespace ClientManagement {

    class ClientManager;

}}

namespace Server { namespace Network {

    class ClientConnection :
        public boost::enable_shared_from_this<ClientConnection>,
        private boost::noncopyable
    {
    private:
        static const unsigned int _bufferSize = 8192;

        ClientManagement::ClientManager& _clientManager;
        boost::asio::io_service& _ioService;
        boost::asio::ip::tcp::socket* _socket;
        Uint8* _data;
        size_t _size;
        size_t _offset;
        size_t _toRead;
        std::queue<std::unique_ptr<Common::Packet>> _toSendPackets;
        bool _connected;
        bool _writeConnected;
        Uint32 _clientId;

    public:
        ClientConnection(boost::asio::ip::tcp::socket* socket, ClientManagement::ClientManager& clientManager);
        ~ClientConnection();
        void SetClientId(Uint32 id);
        void SendPacket(Common::Packet* packet)
        {
            this->_ioService.dispatch(std::bind(&ClientConnection::_SendPacket, this->shared_from_this(), packet));
        }
        void Shutdown()
        {
            this->_ioService.dispatch(std::bind(&ClientConnection::_Shutdown, this->shared_from_this()));
        }
        void ConnectRead()
        {
            this->_ioService.dispatch(std::bind(&ClientConnection::_ConnectRead, this->shared_from_this()));
        }

    private:
        void _Shutdown();
        void _HandleError(boost::system::error_code const& error);
        void _SendPacket(Common::Packet* packet);
        void _ConnectRead();
        void _ConnectWrite();
        void _HandleRead(boost::system::error_code const error,
                         std::size_t transferredBytes);
        void _HandleWrite(boost::shared_ptr<Common::Packet> packetSent,
                          boost::system::error_code const error,
                          std::size_t bytes_transferred);
    };

}}

#endif
