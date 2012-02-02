#ifndef __SERVER_CLIENTCONNECTION_HPP__
#define __SERVER_CLIENTCONNECTION_HPP__

#include <queue>

#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>

#include "common/PacketPtr.hpp"

#include "server/SocketPtr.hpp"

namespace Server {

    class ClientConnection :
        public boost::enable_shared_from_this<ClientConnection>,
        private boost::noncopyable
    {
    public:
        typedef boost::function<void(Common::PacketPtr&)> HandlePacketCallback;
        typedef boost::function<void(boost::system::error_code const&)> HandleErrorCallback;

    private:
        boost::asio::io_service&        _ioService;
        SocketPtr                       _socket;
        Uint8*                          _data;
        size_t                          _size;
        size_t                          _offset;
        size_t                          _toRead;
        std::queue<Common::PacketPtr>   _toSendPackets;
        HandlePacketCallback            _onPacket;
        HandleErrorCallback             _onError;
        volatile bool                   _writeConnected;

    public:
        ClientConnection(SocketPtr& socket, HandlePacketCallback onPacket, HandleErrorCallback onError);
        ~ClientConnection();
        void SendPacket(Common::PacketPtr packet)
        {
            this->_ioService.dispatch(std::bind(&ClientConnection::_SendPacket, this, packet.release()));
        }

        void Shutdown()
        {
            this->_ioService.dispatch(std::bind(&ClientConnection::_Shutdown, this));
        }

        void ConnectRead()
        {
            this->_ioService.dispatch(std::bind(&ClientConnection::_ConnectRead, this));
        }
    private:
        void _Shutdown();
        void _SendPacket(Common::Packet* packet);
        void _ConnectRead();
        void _ConnectWrite();
        void _HandleRead(boost::system::error_code const error,
                         std::size_t transferredBytes);
        void _HandleWrite(boost::shared_ptr<Common::Packet> packetSent,
                          boost::system::error_code const error,
                          std::size_t bytes_transferred);
    };

}

#endif
