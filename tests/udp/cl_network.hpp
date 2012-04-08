#ifndef __CL_NETWORK_HPP__
#define __CL_NETWORK_HPP__

namespace Common {
    class Packet;
}

namespace Tools {
    class ByteArray;
}

namespace cl {
    class UdpPacket;
}

namespace cl {

    class Network :
        private boost::noncopyable
    {
    private:
        boost::asio::io_service _ioService;
        boost::asio::ip::tcp::socket _socket;
        boost::asio::ip::udp::socket _udpSocket;
        boost::asio::ip::udp::endpoint _udpSenderEndpoint;
        boost::thread* _thread;
        std::vector<char> _sizeBuffer;
        std::vector<char> _dataBuffer;
        std::list<Common::Packet*> _outQueue;
        bool _sending;
        std::string _host;
        std::string _port;
        bool _isConnected;
        bool _isRunning;
        std::string _lastError;
        float _loading;
        Uint32 _id;

        std::list<UdpPacket*> _outQueueUdp;
        bool _sendingUdp;

        std::vector<char> _udpDataBuffer;

        struct {
            bool canReceive;
            bool canSend;

            bool serverCanReceive;
            bool passThroughActive;
            unsigned int passThroughCount;

            bool ptOkSent;
        } _udpStatus;


    public:
        Network();
        ~Network();
        float GetLoadingProgression();
        void Connect(std::string const& host, std::string const& port);
        void Stop();
        void SendPacket(std::unique_ptr<Common::Packet> packet);
        void SendUdpPacket(std::unique_ptr<UdpPacket> packet);
        std::string const& GetHost() const { return this->_host; }
        std::string const& GetPort() const { return this->_port; }
        bool IsRunning() const { return this->_isRunning; }
        bool IsConnected() const { return this->_isConnected; }
        std::string GetLastError() const { return this->_lastError; }
        void PassThrough1();

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

        void _HandlePacket(Tools::ByteArray* packet);
        void _TimedDispatch(std::function<void(void)> fx, Uint32 ms);
        void _ExecDispatch(std::function<void(void)>& message,
                std::shared_ptr<boost::asio::deadline_timer>,
                boost::system::error_code const& error);
    };

}

#endif
