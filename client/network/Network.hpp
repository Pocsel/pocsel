#ifndef __CLIENT_NETWORK_NETWORK_HPP__
#define __CLIENT_NETWORK_NETWORK_HPP__

namespace Common {
    class Packet;
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
        std::list<Common::Packet*> _outQueue; // protected by _outMutex
        std::list<Common::Packet*> _inQueue; // protected by _inMutex
        boost::mutex _outMutex;
        boost::mutex _inMutex;
        mutable boost::mutex _metaMutex;
        bool _sending; // protected by _inMutex
        std::string _host;
        std::string _port;
        bool _isConnected; // protected by _metaMutex
        bool _isRunning; // protected by _metaMutex
        std::string _lastError; // protected by _metaMutex
        float _loading;

    public:
        Network();
        ~Network();
        float GetLoadingProgression();
        void Connect(std::string const& host, std::string const& port);
        void Stop();
        void SendPacket(std::unique_ptr<Common::Packet> packet);
        std::list<Common::Packet*> GetInPackets();
        std::string const& GetHost() const { return this->_host; }
        std::string const& GetPort() const { return this->_port; }
        bool IsRunning() const { boost::unique_lock<boost::mutex> lock(this->_metaMutex); return this->_isRunning; }
        bool IsConnected() const { boost::unique_lock<boost::mutex> lock(this->_metaMutex); return this->_isConnected; }
        std::string GetLastError() const { boost::unique_lock<boost::mutex> lock(this->_metaMutex); return this->_lastError; }
    private:
        void _Run();
        void _Connect(std::string host, std::string port);
        void _CloseSocket();
        void _DisconnectedByNetwork(std::string const& error);
        void _SendNext();
        void _HandleWrite(boost::system::error_code const& error);
        void _ReceivePacketSize();
        void _HandleReceivePacketSize(const boost::system::error_code& error);
        void _ReceivePacketContent(unsigned int size);
        void _HandleReceivePacketContent(const boost::system::error_code& error);
    };

}}

#endif
