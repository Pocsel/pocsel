#ifndef __CLIENT_NETWORK_HPP__
#define __CLIENT_NETWORK_HPP__

#include <tuple>
#include <functional>
#include <string>
#include <queue>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "common/PacketPtr.hpp"


namespace Client {

    class Network :
        private boost::noncopyable
    {
    public:
        typedef std::function<void(Common::PacketPtr&)> PacketCallback;
        typedef std::function<void(std::string const&)> ErrorCallback;
        typedef std::function<void(void)> SentCallback;
        typedef std::tuple<Common::Packet*, SentCallback> ToSend;

    private:
        PacketCallback _onPacket;
        ErrorCallback _onError;
        boost::asio::io_service _ioService;
        boost::asio::ip::tcp::socket* _socket;
        std::vector<char> _sizeBuffer;
        std::vector<char> _dataBuffer;
        std::queue<ToSend> _outQueue;
        bool _sending;
        mutable boost::mutex _mutex;
        std::string _host;

    public:
        Network(PacketCallback packetCallback,
                ErrorCallback errorCallback);
        ~Network();
        void Connect(std::string const& host, Uint16 port);
        void Run();
        void Stop();
        void SendPacket(Common::PacketPtr packet, SentCallback cb = 0);
        std::string const& GetHost() const { return this->_host; }
    private:
        void _Disconnect(boost::system::error_code const& error);
        void _SendNext();
        void _HandleWrite(boost::system::error_code const& error);
        void _ReceivePacketSize();
        void _HandleReceivePacketSize(const boost::system::error_code& error);
        void _ReceivePacketContent(unsigned int size);
        void _HandleReceivePacketContent(const boost::system::error_code& error);
    };

}

#endif


