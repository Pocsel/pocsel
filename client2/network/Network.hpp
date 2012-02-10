#ifndef __CLIENT_NETWORK_NETWORK_HPP__
#define __CLIENT_NETWORK_NETWORK_HPP__

#include <list>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

namespace Common {
    class Packet;
}
namespace Client {
    class Client;
}

namespace Client { namespace Network {

    class Network :
        private boost::noncopyable
    {
    private:
        boost::asio::io_service _ioService;
        boost::asio::ip::tcp::socket _socket;
        boost::thread* _thread;
        std::vector<char> _sizeBuffer;
        std::vector<char> _dataBuffer;
        std::list<Common::Packet*> _outQueue;
        std::list<Common::Packet*> _inQueue;
        boost::mutex _outMutex;
        boost::mutex _inMutex;
        bool _sending;
        std::string _host;
        std::string _port;
        bool _isConnected;

    public:
        Network(Client& client);
        ~Network();
        void Connect(std::string const& host, std::string const& port);
        void Run();
        void Stop();
        void SendPacket(std::unique_ptr<Common::Packet> packet);
        std::list<Common::Packet*> ProcessInPackets();
        std::string const& GetHost() const;
        std::string const& GetPort() const;
        bool IsConnected() const;
    private:
        void _Disconnect();
        void _SendNext();
        void _HandleWrite(boost::system::error_code const& error);
        void _ReceivePacketSize();
        void _HandleReceivePacketSize(const boost::system::error_code& error);
        void _ReceivePacketContent(unsigned int size);
        void _HandleReceivePacketContent(const boost::system::error_code& error);
    };

}}

#endif
