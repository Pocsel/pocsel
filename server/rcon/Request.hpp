#ifndef __SERVER_RCON_REQUEST_HPP__
#define __SERVER_RCON_REQUEST_HPP__

namespace Server {
    class Server;
}

namespace Server { namespace Rcon {

    class SessionManager;

    class Request :
        private boost::noncopyable
    {
    private:
        Server& _server;
        boost::asio::ip::tcp::socket* _socket;
        SessionManager& _sessionManager;
        boost::asio::streambuf _buffer;

    public:
        Request(Server& server, boost::asio::ip::tcp::socket* socket, SessionManager& sessionManager);
        ~Request();
    private:
        void _ReadHttpHeader();
        void _HttpHeaderReceived(boost::system::error_code const& error, std::size_t size);
    };

}}

#endif
