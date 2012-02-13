#ifndef __SERVER_NETWORK_CLIENTCONNECTION_HPP__
#define __SERVER_NETWORK_CLIENTCONNECTION_HPP__

#include "common/Packet.hpp"

namespace Server { namespace ClientManagement {

    class ClientManager;

}}

namespace Server { namespace Network {

    class ClientConnection :
        public boost::enable_shared_from_this<ClientConnection>,
        private boost::noncopyable
    {
    public:
        typedef std::function<void(void)> ErrorCallback;
        typedef std::function<void(std::unique_ptr<Common::Packet>&)> PacketCallback;

    private:
        static const unsigned int _bufferSize = 8192;

        boost::asio::io_service& _ioService;
        boost::asio::ip::tcp::socket* _socket;
        Uint8* _data;
        size_t _size;
        size_t _offset;
        size_t _toRead;
        std::queue<std::unique_ptr<Common::Packet>> _toSendPackets;
        bool _connected;
        bool _writeConnected;
        ErrorCallback _errorCallback;
        PacketCallback _packetCallback;

    public:
        ClientConnection(boost::asio::ip::tcp::socket* socket);
        ~ClientConnection();
        void SetCallbacks(ErrorCallback errorCallback, PacketCallback packetCallback);
        void SendPacket(std::unique_ptr<Common::Packet> packet)
        {
            this->_ioService.dispatch(std::bind(&ClientConnection::_SendPacket, this->shared_from_this(), packet.release()));
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
