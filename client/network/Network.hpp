#ifndef __CLIENT_NETWORK_NETWORK_HPP__
#define __CLIENT_NETWORK_NETWORK_HPP__

#include "common/Packet.hpp"

namespace Client { namespace Network {

    class UdpPacket;

    class Network :
        private boost::noncopyable
    {
    private:
        struct InQueue
        {
        private:
            std::list<Tools::ByteArray*> _list;
            boost::mutex _mutex;
        public:
            void PushPacket(Tools::ByteArray* p)
            {
                boost::lock_guard<boost::mutex> lock(_mutex);
                _list.push_back(p);
            }
            std::list<Tools::ByteArray*> StealPackets()
            {
                boost::lock_guard<boost::mutex> lock(_mutex);
                return std::move(_list);
            }
            void Clear()
            {
                std::for_each(this->_list.begin(), this->_list.end(), [](Tools::ByteArray* p) { Tools::Delete(p); });
                this->_list.clear();
            }
            ~InQueue()
            {
                this->Clear();
            }
        };
    private:
        boost::asio::io_service _ioService;
        boost::asio::ip::tcp::socket _socket;
        boost::asio::ip::udp::socket _udpSocket;
        boost::asio::ip::udp::socket _udpReceiveSocket;
        boost::thread* _thread;
        std::vector<char> _sizeBuffer;
        std::vector<char> _dataBuffer;
        std::list<Common::Packet*> _outQueue;
        InQueue _inQueue;
        bool _sending;
        std::string _host;
        std::string _port;
        bool _isConnected;
        bool _isRunning;
        std::string _lastError;
        float _loading;

        std::list<UdpPacket*> _outQueueUdp;
        bool _sendingUdp;
        bool _udp;

        std::vector<char> _udpDataBuffer;
        bool _udpReceive;

    public:
        Network();
        ~Network();
        float GetLoadingProgression();
        void Connect(std::string const& host, std::string const& port);
        void Stop();
        void SendPacket(std::unique_ptr<Common::Packet> packet);
        void SendUdpPacket(std::unique_ptr<UdpPacket> packet);
        std::list<Tools::ByteArray*> GetInPackets();
        std::string const& GetHost() const { return this->_host; }
        std::string const& GetPort() const { return this->_port; }
        bool IsRunning() const { return this->_isRunning; }
        bool IsConnected() const { return this->_isConnected; }
        std::string GetLastError() const { return this->_lastError; }

    private:
        void _SendPacket(std::shared_ptr<Common::Packet> p);
        void _SendUdpPacket(std::shared_ptr<UdpPacket> packet);
        void _Run();
        void _Connect(std::string host, std::string port);
        void _CloseSocket();
        void _DisconnectedByNetwork(std::string const& error);
        void _SendNext();
        void _SendNextUdp();
        void _HandleWrite(boost::system::error_code const& error);
        void _HandleWriteUdp(boost::system::error_code const& error);
        void _ReceiveUdpPacket();
        void _HandleReceiveUdpPacket(boost::system::error_code const& e, std::size_t size);
        void _ReceivePacketSize();
        void _HandleReceivePacketSize(const boost::system::error_code& error);
        void _ReceivePacketContent(unsigned int size);
        void _HandleReceivePacketContent(const boost::system::error_code& error);
    };

}}

#endif
